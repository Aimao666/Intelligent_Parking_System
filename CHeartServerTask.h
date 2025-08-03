#pragma once
#include "CBaseTask.h"
class CHeartServerTask :
    public CBaseTask
{
public:
	CHeartServerTask(int fd, char* data, size_t len);
	~CHeartServerTask() = default;
private:

	// 通过 CBaseTask 继承
	virtual void work() override;
};

