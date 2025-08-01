#pragma once
#include "CBaseTask.h"
class CUploadVideoTask :
    public CBaseTask
{
public:
	CUploadVideoTask(int fd, char* data, size_t len);
	~CUploadVideoTask() = default;
private:

	// 通过 CBaseTask 继承
	virtual void work() override;
};


