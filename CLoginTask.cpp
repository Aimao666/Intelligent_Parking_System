#include "CLoginTask.h"

CLoginTask::CLoginTask(int fd, char* data, size_t len)
	:CBaseTask(fd,data,len)
{
}

CLoginTask::~CLoginTask()
{
}

void CLoginTask::work()
{
	cout << this->taskData << "正在执行" << endl;
	if (this->dataLen <= 0) {
		cout << "请求体长度小于0，异常" << endl;
		return;
	}
	//数据解析
	HEAD head;
	//拿crc码存一下fd，因为进来的时候已经校验过了，这个字段暂时没啥用
	head.crc = this->clientFd;
	LoginRequest loginRequest;
	memcpy(&head, taskData, sizeof(HEAD));
	memcpy(&loginRequest, taskData + sizeof(HEAD), head.bussinessLength);
	cout << "登录请求-账号:" << loginRequest.account <<"	密码:" << loginRequest.password << endl;
	//数据放到共享内存
	IPCManager::getInstance()->saveData(taskData, dataLen, 1);
}
