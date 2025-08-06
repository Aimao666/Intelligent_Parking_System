#include <cstdio>
#include <string>
#include <ctime>
#include "CThreadPool.h"
#include "CLoginTask.h"
#include "CEpollServer.h"
#include "IPCManager.h"
#include "protocol.h"
#include "MessageCodeSender.h"
#include "RrConfig.h"
#include <libgen.h>
using namespace std;
//服务器压力测试，模拟上百客户端连接和上千次的任务请求
void* thread_Function(void* arg) {

    CEpollServer* server = static_cast<CEpollServer*>(arg);
    CThreadPool* pool = server->getPool();
    int socketfd = server->getSocketfd();
    sleep(1);
    int clientNums;
    int everyClientTaskNums;
    //压测有点问题，先不做了
    while(0){
        cout << "输入客户端连接数(数字,5-300)以开始压力测试：" << endl;
        cin >> clientNums;
        cout << "输入每个客户端要发起的登录任务数(数字,5-50)以开始压力测试：" << endl;
        cin >> everyClientTaskNums;
        if (clientNums < 5 || everyClientTaskNums < 5 || clientNums>300 || everyClientTaskNums>50)break;

        const int NUM_CLIENTS = clientNums;
        const int REQUESTS_PER_CLIENT = everyClientTaskNums; // 每个客户端everyClientTaskNums个请求
        const int TOTAL_REQUESTS = NUM_CLIENTS * REQUESTS_PER_CLIENT;

        sockaddr_in sockadd;
        sockadd.sin_port = htons(10001);
        sockadd.sin_family = AF_INET;
        sockadd.sin_addr.s_addr = inet_addr("192.168.204.144");
        // 创建客户端连接池
        vector<int> clientSockets;
        for (int i = 0; i < NUM_CLIENTS; i++) {
            int sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0) {
                perror("socket创建失败");
                continue;
            }
            // 设置非阻塞（可选）
            int flags = fcntl(sockfd, F_GETFL, 0);
            fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
            // 异步连接
            connect(sockfd, (sockaddr*)&sockadd, sizeof(sockadd));
            clientSockets.push_back(sockfd);
            cout << "创建客户端连接 [" << i << "] FD=" << sockfd << endl;
        } 
        // 等待所有连接完成（简化处理）
        sleep(5);



        long tel = 17716777930;
        for (int req = 0; req < REQUESTS_PER_CLIENT; req++) {
            //每一轮每个客户端都要发请求
            for (int clientIdx = 0; clientIdx < NUM_CLIENTS; clientIdx++, tel++) {
                // 获取客户端套接字
                int clientFd = clientSockets[clientIdx];
                HEAD head;
                LoginRequest request;
                head.bussinessLength = sizeof(LoginRequest);
                head.bussinessType = 1;
                //strcpy(request.account, "17716777930");
                sprintf(request.account, "%ld", tel);
                strcpy(request.password, "e10adc3949ba59abbe56e057f20f883e");
                head.crc = CTools::crc32((uint8_t*)&request, sizeof(LoginRequest));
                char buf[sizeof(HEAD) + sizeof(LoginRequest)] = { 0 };
                memcpy(buf, &head, sizeof(HEAD));
                memcpy(buf + sizeof(HEAD), &request, sizeof(request));

                cout << "添加任务" << clientIdx * REQUESTS_PER_CLIENT + req + 1 << endl;
                pool->pushTask(unique_ptr<CBaseTask>(new CLoginTask(clientFd, buf, sizeof(buf))));
                if ((req * NUM_CLIENTS + clientIdx + 1) % 300 == 0)sleep(1);
            }
            
            
        }
        cout << "睡眠30s等待任务完成";
        sleep(30);
        cout << "所有任务已提交，共 " << NUM_CLIENTS * REQUESTS_PER_CLIENT << " 个请求" << endl;
        // 清理客户端连接
        for (int sockfd : clientSockets) {
            close(sockfd);
        }
        clientSockets.clear();
    }
    return nullptr;
}

//心跳服务线程
void* heartServiceTimer(void* arg) {
    const int TIME_SPAN = 60 * 10;//定时任务时间跨度
    CEpollServer* epollServer = static_cast<CEpollServer*>(arg);
    sleep(TIME_SPAN);
    while (1) {
        //string nowTime = CTools::getDatetime();
        time_t nowTimeStamp = CTools::convertTimeStr2TimeStamp(CTools::getDatetime());
        cout << "===========心跳超时下线============" << endl;
        for (auto& pair : DataManager::heartServiceMap) {
            time_t lastTimeStamp = CTools::convertTimeStr2TimeStamp(pair.second->lastServerTime);
            time_t diffTime = nowTimeStamp - lastTimeStamp;
            if (lastTimeStamp <= 0) {
                cerr << "错误的时间戳：" << lastTimeStamp << " fd=" << pair.first << " account=" << pair.second->account << endl;
                continue;
            }
            else if (diffTime > TIME_SPAN) {
                //超时踢下线
                HEAD head;
                head.bussinessType = 31;
                head.crc = 0;//表示被前置主动踢出
                OffLineRequest body;
                head.bussinessLength = sizeof(body);
                strcpy(body.account, pair.second->account.c_str());
                char buf[sizeof(head) + sizeof(body)];
                memcpy(buf, &head, sizeof(HEAD));
                memcpy(buf+sizeof(HEAD), &body, sizeof(body));
                auto task = CTaskFactory::getInstance()->createTask(pair.first, head.bussinessType, buf, sizeof(buf));//主动踢出任务
                if (task) {
                    CThreadPool::getInstance()->pushTask(std::move(task));
                }
                head.bussinessType++;
                CommonBack bodyBack;
                head.bussinessLength = sizeof(bodyBack);
                bodyBack.flag = 1;
                strcpy(bodyBack.message, "心跳超时下线");
                head.crc = CTools::crc32((uint8_t*)&bodyBack, sizeof(bodyBack));
                char buffer[sizeof(head) + sizeof(bodyBack)];
                memcpy(buffer, &head, sizeof(HEAD));
                memcpy(buffer + sizeof(HEAD), &body, sizeof(bodyBack));
                int res = write(pair.first, buffer, sizeof(buffer));
                if (res > 0) {
                    cout << "发送成功,业务类型:" << head.bussinessType << " (预计,实际)字节数：(" << 
                        sizeof(buffer)<< "," << res << ")" << endl;
                }
                else {
                    perror("心跳超时 write err:");
                }
                epollServer->closeClient(pair.first);
                cout << "心跳超时下线fd=" << pair.first << endl;
            }
        }
        cout << "===================================" << endl;
        time_t nowTimeStamp2 = CTools::convertTimeStr2TimeStamp(CTools::getDatetime());
        sleep(TIME_SPAN-(nowTimeStamp2-nowTimeStamp));
    }
}
std::string getConfigPath() {
    char exePath[256];
    ssize_t count = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (count <= 0) {
        return "config.ini"; // 回退到当前目录
    }
    exePath[count] = '\0';

    // 获取可执行文件所在目录
    char* exeDir = dirname(exePath);

    // 构建配置文件路径
    std::string configPath = std::string(exeDir) + "/../config.ini";

    // 检查文件是否存在
    if (access(configPath.c_str(), R_OK) == 0) {
        return configPath;
    }

    // 尝试其他位置
    configPath = std::string(exeDir) + "/../../config.ini";
    if (access(configPath.c_str(), R_OK) == 0) {
        return configPath;
    }

    // 最后尝试当前目录
    return "config.ini";
}
int loadConfig() {
    char path[256];
    cout << "pwd=" << getcwd(path, 256) << endl;
    rr::RrConfig config;
    string configPath = getConfigPath();
    cout << "configPath=" << configPath << endl;
    bool ret = config.ReadConfig(configPath);
    if (ret == false) {
        printf("ReadConfig is Error,Cfg=%s", configPath.c_str());
        return 0;
    }
    std::string apiAccount = config.ReadString("ihuyi", "apiAccount", "");
    std::string apiKey = config.ReadString("ihuyi", "apiKey", "");

    std::cout << "apiAccount=" << apiAccount << std::endl;
    std::cout << "apiKey=" << apiKey << std::endl;
    MessageCodeSender::getInstance().init(apiAccount, apiKey);
    return 1;
}
int main(int argc, char* argv[])
{
    //加载配置文件
    if (!loadConfig())
    {
        cout << "配置文件加载失败" << endl;
        return 0;
    }
    //互斥锁初始化
    pthread_mutex_init(&DataManager::mutex,NULL);
	srand(unsigned(time(0)));
    
    //IPC初始化
    IPCManager* ipc = IPCManager::getInstance();
    int msgid = ipc->initMsg(20001);
    int semid = ipc->initSem(20001, 1000, 1);
    int block_num = ipc->getNums_sems();//信号量数组大小，对应共享内存被拆分成多少块
    int shmid = ipc->initShm(20001, block_num * (sizeof(int) + MAX_BODY_LENGTH));
    if (msgid < 0 || semid < 0 || shmid < 0) {
        cout << "ipc分配失败,shmid=" << shmid << " semid=" << semid << " msgid=" << msgid << endl;
        return 0;
    }
    void* shmaddr = shmat(shmid, NULL, 0);//用于接收共享内存块起始地址
    if (shmaddr == (void*)-1) {
        perror("shmat 失败");
        cout << "无法连接共享内存，程序退出,shmid="<< shmid << endl;
        return -1;
    }
    shmdt(shmaddr);
    CEpollServer* epollServer = new CEpollServer(10001, 5);
    //压力测试线程
    pthread_t tid;
    pthread_create(&tid, NULL, thread_Function, epollServer);
    //心跳服务线程
    pthread_t heartTid;
    pthread_create(&heartTid, NULL, heartServiceTimer, epollServer);
    try {
        epollServer->work();
    }
    catch(exception e){
        cerr << e.what() << endl;
        cout << "epoll结束程序终止" << endl;
    }
    //while (1) {

    //}
    return 0;
}

/*
    int main() {

    rr::RrConfig config;
    bool ret = config.ReadConfig("config.ini");
    if (ret == false) {
        printf("ReadConfig is Error,Cfg=%s", "config.ini");
        return 1;
    }
    std::string HostName = config.ReadString("MYSQL", "HostName", "");
    int Port = config.ReadInt("MYSQL", "Port", 0);
    std::string UserName = config.ReadString("MYSQL", "UserName", "");

    std::cout << "HostName=" << HostName << std::endl;
    std::cout << "Port=" << Port << std::endl;
    std::cout << "UserName=" << UserName << std::endl;

    return 0;
}
*/