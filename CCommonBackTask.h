#pragma once
#include "CBaseTask.h"
class CCommonBackTask
	:public CBaseTask
{
public:
	CCommonBackTask(int fd, char* data, size_t len);
	~CCommonBackTask();
private:

	// 通过 CBaseTask 继承
	virtual void work() override;
};
