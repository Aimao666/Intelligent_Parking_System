#include <cstdio>
#include <string>
#include <ctime>
#include "CThreadPool.h"
#include "CLoginTask.h"
#include "CEpollServer.h"
#include "IPCManager.h"
#include "protocol.h"
using namespace std;
int main(int argc, char* argv[])
{
    printf("参数个数：%d\n", argc);
    printf("程序名称：%s\n", argv[0]);
	srand(unsigned(time(0)));
    CEpollServer* epollServer = new CEpollServer(10001, 5);
    epollServer->work();

    IPCManager* ipc = IPCManager::getInstance();
    int msgid = ipc->initMsg(20001);
    int semid = ipc->initSem(20001, 24, 1);
    int block_num = ipc->getNums_sems();//信号量数组大小，对应共享内存被拆分成多少块
    int shmid = ipc->initShm(20001, block_num * (sizeof(int) + sizeof(MAX_BODY_LENGTH)));
    if (msgid < 0 || semid < 0 || shmid < 0) {
        cout << "ipc分配失败" << endl;
        return 0;
    }
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