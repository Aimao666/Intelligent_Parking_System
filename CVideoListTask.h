#pragma once
#include "CBaseTask.h"
class CVideoListTask :
    public CBaseTask
{
public:
	CVideoListTask(int fd, char* data, size_t len);
	~CVideoListTask() = default;
private:

	// 通过 CBaseTask 继承
	virtual void work() override;
};
