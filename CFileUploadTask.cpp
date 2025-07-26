#include "CFileUploadTask.h"

CFileUploadTask::CFileUploadTask(int fd, char* data, size_t len)
	:CBaseTask(fd,data,len)
{
}


//文件上传业务，接收一个文件碎片
void CFileUploadTask::work()
{
	cout << "CFileUploadTask正在执行" << endl;
	//业务数据直接存共享内存
	CBaseTask::work();
}
