#pragma once
#include "CBaseTask.h"
class CUploadPlayinfoTask :
    public CBaseTask
{
public:
	CUploadPlayinfoTask(int fd, char* data, size_t len);
	~CUploadPlayinfoTask() = default;
private:

	// 通过 CBaseTask 继承
	virtual void work() override;
};
