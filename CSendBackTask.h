#pragma once
#include "CBaseTask.h"
class CSendBackTask 
	: public CBaseTask
{
public:
	CSendBackTask(int fd, char* data, size_t len);
	~CSendBackTask() = default;
private:

	// 通过 CBaseTask 继承
	virtual void work() override;
};
