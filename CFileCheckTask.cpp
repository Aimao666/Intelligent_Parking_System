#include "CFileCheckTask.h"

CFileCheckTask::CFileCheckTask(int fd, char* data, size_t len)
	:CBaseTask(fd,data,len)
{
}


//一个文件发送完毕后客户端发来的确认包
void CFileCheckTask::work()
{
	cout << "CFileCheckTask文件上传确认包" << endl;
	CBaseTask::work();
}
