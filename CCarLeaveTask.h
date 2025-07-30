#pragma once
#include "CBaseTask.h"
class CCarLeaveTask :
    public CBaseTask
{
public:
	CCarLeaveTask(int fd, char* data, size_t len);
	~CCarLeaveTask() = default;
private:

	// 通过 CBaseTask 继承
	virtual void work() override;
};
