#include <cstdio>
#include <string>
#include <ctime>
#include "CThreadPool.h"
#include "CLoginTask.h"
#include "CEpollServer.h"
using namespace std;
int main(int argc, char* argv[])
{
    printf("参数个数：%d\n", argc);
    printf("程序名称：%s\n", argv[0]);
	srand(unsigned(time(0)));
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