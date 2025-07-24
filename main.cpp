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
int main(int argc, char* argv[])
{
    printf("参数个数：%d\n", argc);
    printf("程序名称：%s\n", argv[0]);
	srand(unsigned(time(0)));
    IPCManager* ipc = IPCManager::getInstance();
    int msgid = ipc->initMsg(20001);
    int semid = ipc->initSem(20001, 48, 1);
    int block_num = ipc->getNums_sems();//信号量数组大小，对应共享内存被拆分成多少块
    int shmid = ipc->initShm(20001, block_num * (sizeof(int) + MAX_BODY_LENGTH));
    if (msgid < 0 || semid < 0 || shmid < 0) {
        cout << "ipc分配失败" << endl;
        return 0;
    }
    void* shmaddr = shmat(shmid, NULL, 0);//用于接收共享内存块起始地址
    if (shmaddr == (void*)-1) {
        perror("shmat 失败");
        cout << "无法连接共享内存，程序退出" << endl;
        return -1;
    }
    shmdt(shmaddr);
    MessageCodeSender::getInstance().init("C65451499", "6bcebac852eaf1c5be1d3318e6c4674b");
    CEpollServer* epollServer = new CEpollServer(10001, 5);
    epollServer->work();

    //CThreadPool* pool = epollServer->getPool();
    //sleep(3);
    //for (int i = 0; i < 10; i++) {
    //    char buf[50] = { 0 };
    //    sprintf(buf, "%s%d", "任务", i + 1);
    //    pool->pushTask(unique_ptr<CBaseTask>(new CLoginTask(1, buf,50)));
    //    cout << "添加任务" << i + 1 << endl;
    //}
    while (1) {

    }
    return 0;
}