#pragma once
#include "CBaseTask.h"
//放行
class CAgreeLeaveTask :
    public CBaseTask
{
public:
	CAgreeLeaveTask(int fd, char* data, size_t len);
	~CAgreeLeaveTask() = default;
private:

	// 通过 CBaseTask 继承
	virtual void work() override;
};
