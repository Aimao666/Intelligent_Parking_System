#include <cstdio>
#include <string>
#include <ctime>
#include "CThreadPool.h"
#include "CLoginTask.h"
#include "CEpollServer.h"
#include "IPCManager.h"
#include "protocol.h"
#include "MessageCodeSender.h"
using namespace std;
//服务器压力测试，模拟上百客户端连接和上千次的任务请求
void* thread_Function(void* arg) {

    CEpollServer* server = (CEpollServer*)arg;
    CThreadPool* pool = server->getPool();
    int socketfd = server->getSocketfd();
    sleep(1);
    int clientNums;
    int everyClientTaskNums;
    while (1) {
        cout << "输入客户端连接数(数字,5-500)以开始压力测试：" << endl;
        cin >> clientNums;
        cout << "输入每个客户端要发起的登录任务数(数字,5-50)以开始压力测试：" << endl;
        cin >> everyClientTaskNums;
        if (clientNums < 5 || everyClientTaskNums < 5)break;

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
        sleep(1);



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
                if ((clientIdx * REQUESTS_PER_CLIENT + req + 1) % 500 == 0)sleep(1);
            }
            
        }
        cout << "睡眠4s等待任务完成";
        sleep(10);
        cout << "所有任务已提交，共 " << NUM_CLIENTS * REQUESTS_PER_CLIENT << " 个请求" << endl;
        // 清理客户端连接
        for (int sockfd : clientSockets) {
            close(sockfd);
        }
    }
    return nullptr;
}

int main(int argc, char* argv[])
{
    printf("参数个数：%d\n", argc);
    printf("程序名称：%s\n", argv[0]);
	srand(unsigned(time(0)));
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
    MessageCodeSender::getInstance().init("C65451499", "6bcebac852eaf1c5be1d3318e6c4674b");
    CEpollServer* epollServer = new CEpollServer(10001, 5);
    pthread_t tid;
    pthread_create(&tid, NULL, thread_Function, epollServer);
    epollServer->work();

    //while (1) {

    //}
    return 0;
}