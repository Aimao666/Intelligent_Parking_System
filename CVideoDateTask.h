#pragma once
#include "CBaseTask.h"
class CVideoDateTask :
    public CBaseTask
{
public:
	CVideoDateTask(int fd, char* data, size_t len);
	~CVideoDateTask() = default;
private:

	// 通过 CBaseTask 继承
	virtual void work() override;
};
