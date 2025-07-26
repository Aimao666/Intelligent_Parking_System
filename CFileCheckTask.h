#pragma once
#include "CBaseTask.h"
class CFileCheckTask :
    public CBaseTask
{
public:
	CFileCheckTask(int fd, char* data, size_t len);
	~CFileCheckTask() = default;
private:

	// 通过 CBaseTask 继承
	virtual void work() override;
};

