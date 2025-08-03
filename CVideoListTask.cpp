#include "CVideoListTask.h"

CVideoListTask::CVideoListTask(int fd, char* data, size_t len)
	:CBaseTask(fd, data, len)
{
}

void CVideoListTask::work()
{
    cout << "CVideoListTask获取视频列表" << endl;
    //其实这里应该做一下基础的数据合法性校验
    //数据解析
    memcpy(&head, taskData, sizeof(HEAD));
    VideoListRequest request;
    memcpy(&request, taskData + sizeof(HEAD), sizeof(request));
    //账号有效性校验
    cout << "account=" << request.account << endl;
    std::regex tel_reg("^1[3456789]\\d{9}$");
    bool ret = std::regex_match(request.account, tel_reg);
    std::cout << (ret ? "account valid" : "account invalid") << std::endl;
    if (!ret) {
        return;
    }

    cout << "+++++++VideoListRequest详细信息+++++++" << endl;
    cout << "account=" << request.account << " dateTime=" << request.dateTime << endl;
    cout << "queryFlag=" << request.queryFlag << " requestPage=" << request.requestPage << endl;
    cout << "++++++++++++++++++++" << endl;
    if (request.requestPage <= 0 || request.queryFlag < 0 || request.queryFlag>1) {
        cout << "数据校验不通过,请求页不得小于等于0，且标志必须是0或1" << endl;
        return;
    }
    CBaseTask::work();

    pthread_mutex_lock(&DataManager::mutex);
    ++DataManager::getVideoListNum;
    pthread_mutex_unlock(&DataManager::mutex);
}
