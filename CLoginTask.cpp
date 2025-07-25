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
	//cout << "CLoginTask:clientFd=" << clientFd << endl;
	LoginRequest request;
	memcpy(&head, taskData, sizeof(HEAD));
	memcpy(&request, taskData + sizeof(HEAD), head.bussinessLength);
	cout << "登录请求-账号:" << request.account <<"	密码:" << request.password << endl;


	//拿crc码存一下fd，因为进来的时候已经校验过了，这个字段暂时没啥用
	head.crc = this->clientFd;
	//head修改了，需要保存到缓冲区，然后写入共享内存
	memcpy(taskData, &head, sizeof(HEAD));
	//数据放到共享内存
	IPCManager::getInstance()->saveData(taskData, dataLen, 1);//1表示前置到后置
}
