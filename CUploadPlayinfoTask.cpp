#include "CUploadPlayinfoTask.h"

CUploadPlayinfoTask::CUploadPlayinfoTask(int fd, char* data, size_t len)
	:CBaseTask(fd, data, len)
{
}

void CUploadPlayinfoTask::work()
{
    cout << "CUploadPlayinfoTask上传视频播放信息" << endl;
    //其实这里应该做一下基础的数据合法性校验
    //数据解析
    memcpy(&head, taskData, sizeof(HEAD));
    UploadPlayinfoRequest request;
    memcpy(&request, taskData + sizeof(HEAD), sizeof(request));
    //账号有效性校验
    cout << "account=" << request.account << endl;
    std::regex tel_reg("^1[3456789]\\d{9}$");
    bool ret = std::regex_match(request.account, tel_reg);
    std::cout << (ret ? "account valid" : "account invalid") << std::endl;
    if (!ret) {
        return;
    }

    cout << "+++++++UploadPlayinfoRequest详细信息+++++++" << endl;
    cout << "account=" << request.account << " currenttime=" << request.currenttime << endl;
    cout << "videoId=" << request.videoId << endl;
    cout << "++++++++++++++++++++" << endl;
    if (request.videoId < 0 ) {
        cout << "数据校验不通过,视频id不得小于0" << endl;
        return;
    }
    CBaseTask::work();

}
