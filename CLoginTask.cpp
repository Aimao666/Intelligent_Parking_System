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
	LoginRequest loginRequest;
	memcpy(&loginRequest, taskData, dataLen);
	cout << "登录请求-账号:" << loginRequest.account <<"	密码:" << loginRequest.pass << endl;
	if (strcmp(loginRequest.account, "admins") == 0&& strcmp(loginRequest.pass, "123456") == 0) {
		//登陆成功
		cout << "登陆成功" << endl;
	}
	else {
		cout << "登陆失败" << endl;

	}
}
