#pragma once
#include "CBaseTask.h"
class CLoginTask :public CBaseTask
{
public:
	CLoginTask(int fd, char* data, size_t len);
	~CLoginTask();
private:

	// 通过 CBaseTask 继承
	virtual void work() override;
};

