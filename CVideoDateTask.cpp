#include "CVideoDateTask.h"

CVideoDateTask::CVideoDateTask(int fd, char* data, size_t len)
	:CBaseTask(fd, data, len)
{
}

void CVideoDateTask::work()
{
    cout << "CVideoDateTask获取存在的视频日期" << endl;
    //其实这里应该做一下基础的数据合法性校验
    //数据解析
    memcpy(&head, taskData, sizeof(HEAD));
    VideoDateRequest request;
    memcpy(&request, taskData + sizeof(HEAD), sizeof(request));
    //账号有效性校验
    cout << "account=" << request.account << endl;
    std::regex tel_reg("^1[3456789]\\d{9}$");
    bool ret = std::regex_match(request.account, tel_reg);
    std::cout << (ret ? "account valid" : "account invalid") << std::endl;
    if (!ret) {
        return;
    }

    cout << "+++++++VideoDateRequest详细信息+++++++" << endl;
    cout << "account=" << request.account << " year=" << request.year << endl;
    cout << "queryFlag=" << request.queryFlag << endl;
    cout << "++++++++++++++++++++" << endl;
    if (request.queryFlag < 0 || request.queryFlag>1||request.year<0) {
        cout << "数据校验不通过,请求标志必须是0或1,且年份不得小于0" << endl;
        return;
    }
    CBaseTask::work();

}
