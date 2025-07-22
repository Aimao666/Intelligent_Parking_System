#pragma once
#include <string.h>
#include <unistd.h>
#include <iostream>
#include "protocol.h"
using namespace std;
class CBaseTask
{
public:
	CBaseTask(int fd, char* data, size_t len);
	virtual~CBaseTask();
	virtual void work()=0;

	int getClientFd() const;

	char* getTaskData() const;

	size_t getDataLen() const;
protected:
	int clientFd;     // 客户端文件描述符
	char* taskData;   // 原始请求数据（包含请求头+请求体）
	size_t dataLen;   // 数据总长度
};

