#include "CParkingInfoTask.h"

CParkingInfoTask::CParkingInfoTask(int fd, char* data, size_t len)
	:CBaseTask(fd, data, len)
{
}

void CParkingInfoTask::work()
{
    cout << "CParkingInfoTask车辆信息查询" << endl;
    //其实这里应该做一下基础的数据合法性校验
    //数据解析
    memcpy(&head, taskData, sizeof(HEAD));
    ParkingInfoRequest request;
    memcpy(&request, taskData + sizeof(HEAD), sizeof(request));
    //账号有效性校验
    cout << "account=" << request.account << endl;
    std::regex tel_reg("^1[3456789]\\d{9}$");
    bool ret = std::regex_match(request.account, tel_reg);
    std::cout << (ret ? "account valid" : "account invalid") << std::endl;
    if (!ret) {
        return;
    }

    cout << "+++++++ParkingInfoRequest详细信息+++++++" << endl;
    cout << "account=" << request.account << " carNumber=" << request.carNumber << endl;
    cout << "currentPage=" << request.currentPage << " entryTime=" << request.entryTime << endl;
    cout << "leaveTime=" << request.leaveTime << " carNumber=" << request.carNumber << endl;
    cout << "++++++++++++++++++++" << endl;
    if (request.currentPage < 0||!(strcmp(request.carNumber, "")&& !strcmp(request.entryTime, "")&& !strcmp(request.leaveTime, ""))) {
        cout << "数据校验不通过,请求页不得小于0且车牌号，入场时间，出场时间不能全为空" << endl;
        return;
    }
    CBaseTask::work();
}
