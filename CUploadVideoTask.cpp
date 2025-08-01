#include "CUploadVideoTask.h"

CUploadVideoTask::CUploadVideoTask(int fd, char* data, size_t len)
	:CBaseTask(fd, data, len)
{
}

void CUploadVideoTask::work()
{
    cout << "CUploadVideoTask上传录制视频信息" << endl;
    //其实这里应该做一下基础的数据合法性校验
    //数据解析
    memcpy(&head, taskData, sizeof(HEAD));
    UploadVideoRequest request;
    memcpy(&request, taskData + sizeof(HEAD), sizeof(request));
    //账号有效性校验
    cout << "account=" << request.account << endl;
    std::regex tel_reg("^1[3456789]\\d{9}$");
    bool ret = std::regex_match(request.account, tel_reg);
    std::cout << (ret ? "account valid" : "account invalid") << std::endl;
    if (!ret) {
        return;
    }

    cout << "+++++++CUploadVideoRequest详细信息+++++++" << endl;
    cout << "account=" << request.account << " createtime=" << request.createtime << endl;
    cout << "vname=" << request.vname << " vpath=" << request.vpath << endl;
    cout << "pname=" << request.pname << " ppath=" << request.ppath << endl;
    cout << "totaltime=" << request.totaltime << endl;
    cout << "++++++++++++++++++++" << endl;
    CBaseTask::work();
}
