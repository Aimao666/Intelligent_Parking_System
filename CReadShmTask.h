#pragma once
#include "CBaseTask.h"
#include "CTaskFactory.h"
#include "CThreadPool.h"
class CReadShmTask :
    public CBaseTask
{
public:
    CReadShmTask(int shmIndex, CThreadPool* pool);
    ~CReadShmTask();
private:
    CThreadPool* pool;

    // 通过 CBaseTask 继承
    virtual void work() override;

};

