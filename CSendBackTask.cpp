#include "CSendBackTask.h"

CSendBackTask::CSendBackTask(int fd, char* data, size_t len)
	:CBaseTask(fd,data,len)
{
}
//给客户端发送返回包，前置直接发，不管内容，内容都在后置已经编辑好了
void CSendBackTask::work()
{
	cout << "CSendBackTask正在执行" << endl; 
	if (this->dataLen <= 0) {
		cout << "请求体长度小于0，异常" << endl;
		return;
	}
	HEAD headBack;
	memcpy(&headBack, this->taskData, sizeof(HEAD));
	headBack.crc = CTools::crc32((uint8_t*)(taskData + sizeof(HEAD)), headBack.bussinessLength);
	memcpy(this->taskData, &headBack, sizeof(HEAD));
	//发送给客户端
	if (write(this->clientFd, this->taskData, this->dataLen) == -1) {
		perror("CSendBackTask write err:");
		cout << "clientFd=" << this->clientFd << endl;
	}
	else {
		cout << "CSendBackTask-发送成功,业务类型:" << headBack.bussinessType << endl;
	}
}
