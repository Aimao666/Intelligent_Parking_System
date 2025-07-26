#include "CLoginTask.h"

CLoginTask::CLoginTask(int fd, char* data, size_t len)
	:CBaseTask(fd,data,len)
{
}


void CLoginTask::work()
{
	cout << "CLoginTask正在执行" << endl;
	CBaseTask::work();
}
