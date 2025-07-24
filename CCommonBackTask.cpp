#include "CCommonBackTask.h"

CCommonBackTask::CCommonBackTask(int fd, char* data, size_t len)
	:CBaseTask(fd,data,len)
{
}

CCommonBackTask::~CCommonBackTask()
{
}

void CCommonBackTask::work()
{
	cout << this->taskData << "正在执行" << endl;
	if (this->dataLen <= 0) {
		cout << "请求体长度小于0，异常" << endl;
		return;
	}
	//数据解析
	HEAD headBack;
	CommonBack bodyBack;
	memcpy(&headBack, taskData, sizeof(HEAD));
	memcpy(&bodyBack, taskData + sizeof(HEAD), headBack.bussinessLength);
	//返回包计算crc校验码
	headBack.crc = CTools::crc32((uint8_t*)&bodyBack, sizeof(bodyBack));
	//返回包写入缓冲区
	char buf[sizeof(HEAD) + sizeof(bodyBack)];
	memcpy(buf, &headBack, sizeof(HEAD));
	memcpy(buf + sizeof(HEAD), &bodyBack, sizeof(bodyBack));
	//发送给客户端
	if (write(clientFd, buf, sizeof(buf)) == -1) {
		perror("CCommonBackTask write err:");
	}
	else {
		cout << "CCommonBackTask-发送成功" << endl;
	}
}
