#pragma once
#include "CBaseTask.h"
class CFileUploadTask :
    public CBaseTask
{
public:
	CFileUploadTask(int fd, char* data, size_t len);
	~CFileUploadTask() = default;
private:

	// 通过 CBaseTask 继承
	virtual void work() override;
};