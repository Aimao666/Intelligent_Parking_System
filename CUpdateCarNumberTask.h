#pragma once
#include "CBaseTask.h"
class CUpdateCarNumberTask :
    public CBaseTask
{
public:
	CUpdateCarNumberTask(int fd, char* data, size_t len);
	~CUpdateCarNumberTask() = default;
private:

	// 通过 CBaseTask 继承
	virtual void work() override;
};