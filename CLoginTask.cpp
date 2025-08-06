#include "CLoginTask.h"

CLoginTask::CLoginTask(int fd, char* data, size_t len)
	:CBaseTask(fd,data,len)
{
}


void CLoginTask::work()
{
	cout << "CLoginTask正在执行" << endl;
	//其实这里应该做一下基础的数据合法性校验
    //数据解析
    memcpy(&head, taskData, sizeof(HEAD));
    LoginRequest request;
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
    pthread_mutex_lock(&DataManager::mutex);
    /*DataManager::heartServiceMap[clientFd]->account = request.account;
    DataManager::heartServiceMap[clientFd]->lastServerTime = CTools::getDatetime();*/
    // 检查条目是否存在且有效
    auto it = DataManager::heartServiceMap.find(clientFd);
    if (it != DataManager::heartServiceMap.end() && it->second != nullptr) {
        it->second->account = request.account;
        it->second->lastServerTime = CTools::getDatetime();
    }
    else {
        // 记录错误日志
        std::cerr << "警告：clientFd=" << clientFd << " 不在心跳映射中" << std::endl;

        // 创建新条目
        DataManager::heartServiceMap[clientFd] = unique_ptr<DataManager::ClientValue>(new DataManager::ClientValue);
        DataManager::heartServiceMap[clientFd]->account = request.account;
        DataManager::heartServiceMap[clientFd]->lastServerTime = CTools::getDatetime();
    }
    pthread_mutex_unlock(&DataManager::mutex);
}
