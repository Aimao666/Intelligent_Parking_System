#include "CFileUploadTask.h"

CFileUploadTask::CFileUploadTask(int fd, char* data, size_t len)
	:CBaseTask(fd,data,len)
{
}


//文件上传业务，接收一个文件碎片
void CFileUploadTask::work()
{
	cout << "CFileUploadTask正在执行" << endl;
    //数据解析
    memcpy(&head, taskData, sizeof(HEAD));
    FileInfoRequest request;
    memcpy(&request, taskData + sizeof(HEAD), sizeof(request));
    //账号有效性校验
    cout << "account=" << request.account << endl;
    std::regex tel_reg("^1[3456789]\\d{9}$");
    bool ret = std::regex_match(request.account, tel_reg);
    std::cout << (ret ? "account valid" : "account invalid") << std::endl;
    if (!ret) {
        return;
    }
    cout << "++++++CFileUploadTask包详细信息++++++" << endl;
    cout << "account=" << request.account << " filename=" << request.filename << endl;
    cout << "有效字节fileLength=" << request.fileLength << " 序号=" << request.fileIndex << endl;
    cout << "totalNumber=" << request.totalNumber << " khdPath=" << request.khdPath << endl;
    cout << "totalLength=" << request.totalLength << " type=" << request.type << endl;
    cout << "++++++++++++++++++++++" << endl;
    if (request.fileLength <= 0 || request.fileLength > request.totalLength || request.fileLength > sizeof(request.context)
        || request.totalNumber<0 || request.fileIndex>request.totalNumber) {
        cout << "数据校验不合法" << endl;
        return;
    }
	//业务数据直接存共享内存
	CBaseTask::work();
}
