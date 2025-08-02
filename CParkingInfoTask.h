#pragma once
#include "CBaseTask.h"
class CParkingInfoTask :
    public CBaseTask
{
public:
	CParkingInfoTask(int fd, char* data, size_t len);
	~CParkingInfoTask() = default;
private:

	// 通过 CBaseTask 继承
	virtual void work() override;
};

