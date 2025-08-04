#pragma once
#include "CBaseTask.h"
class COffLineTask :
    public CBaseTask
{
public:
	COffLineTask(int fd, char* data, size_t len);
	~COffLineTask() = default;
private:

	// 通过 CBaseTask 继承
	virtual void work() override;
};

