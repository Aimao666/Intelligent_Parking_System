#pragma once
#include "CBaseTask.h"
#include "DataManager.h"

#include "MessageCodeSender.h"
class SendCodeTask :
    public CBaseTask
{
public:
	SendCodeTask(int fd, char* data, size_t len);
	~SendCodeTask() = default;
private:

	// 通过 CBaseTask 继承
	virtual void work() override;
};

