#include "CCarEntryTask.h"
CCarEntryTask::CCarEntryTask(int fd, char* data, size_t len)
	:CBaseTask(fd, data, len)
{
}

void CCarEntryTask::work()
{
	cout << "CCarEntryTask正在执行" << endl;
	//其实这里应该做一下基础的数据合法性校验
    //数据解析
    memcpy(&head, taskData, sizeof(HEAD));
    CarEntryRequest request;
    memcpy(&request, taskData + sizeof(HEAD), sizeof(request));
    //账号有效性校验
    cout << "account=" << request.account << endl;
    std::regex tel_reg("^1[3456789]\\d{9}$");
    bool ret = std::regex_match(request.account, tel_reg);
    std::cout << (ret ? "account valid" : "account invalid") << std::endl;
    if (!ret) {
        return;
    }
	CBaseTask::work();
}
