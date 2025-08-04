#include "COffLineTask.h"

COffLineTask::COffLineTask(int fd, char* data, size_t len) :CBaseTask(fd, data, len)
{
}

void COffLineTask::work()
{
    cout << "COffLineTask客户端下线" << endl;
    //其实这里应该做一下基础的数据合法性校验
    //数据解析
    memcpy(&head, taskData, sizeof(HEAD));
    OffLineRequest request;
    memcpy(&request, taskData + sizeof(HEAD), sizeof(request));
    //账号有效性校验
    cout << "account=" << request.account << endl;
    std::regex tel_reg("^1[3456789]\\d{9}$");
    bool ret = std::regex_match(request.account, tel_reg);
    std::cout << (ret ? "account valid" : "account invalid") << std::endl;
    if (!ret) {
        return;
    }
    //主动下线前置什么都不做，传递给后置服务器处理，客户端在真正下线时会触发epoll走closeClient函数，从而清理前置数据

    cout << "+++++++OffLineRequest详细信息+++++++" << endl;
    cout << "account=" << request.account << endl;
    cout << "++++++++++++++++++++" << endl;
    CBaseTask::work();
}
