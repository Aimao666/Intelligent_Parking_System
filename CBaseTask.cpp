#include "CBaseTask.h"

CBaseTask::CBaseTask(int fd, char* data, size_t len)
    : clientFd(fd), dataLen(len) {
    if (len > 0) {
        taskData = new char[len];
        memcpy(taskData, data, len);
    }
    else {
        dataLen = 0;
        taskData = nullptr;
    }
}

CBaseTask::CBaseTask(int shmIndex)
{
    this->shmIndex = shmIndex;
    dataLen = 0;
    taskData = nullptr;
}
CBaseTask::~CBaseTask()
{
    if (dataLen > 0&& taskData) {
        delete[] taskData;
    }
}
//实现了基本的将业务数据存放到共享内存
void CBaseTask::work()
{
    if (this->dataLen <= 0) {
        cout << "dataLen数据长度小于0，无法将数据从前置拷贝到共享内存" << endl;
        return;
    }
    //数据解析
    HEAD head;
    memcpy(&head, taskData, sizeof(HEAD));
    //拿crc码存一下客户端fd，因为进来的时候已经校验过了，这个字段暂时没啥用
    head.crc = clientFd;
    //head修改了，需要保存到缓冲区，然后写入共享内存
    memcpy(taskData, &head, sizeof(HEAD));
    //数据放到共享内存
    IPCManager::getInstance()->saveData(taskData, dataLen, 1);//1表示前置到后置
}

int CBaseTask::getClientFd() const
{
    return clientFd;
}

char* CBaseTask::getTaskData() const
{
    return taskData;
}

size_t CBaseTask::getDataLen() const
{
    return dataLen;
}

