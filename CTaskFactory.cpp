#include "CTaskFactory.h"
CTaskFactory* CTaskFactory::instance = nullptr;
pthread_mutex_t CTaskFactory::mutex;

CTaskFactory* CTaskFactory::getInstance()
{
	pthread_mutex_lock(&mutex);
	if (instance == nullptr) {
		instance = new CTaskFactory();
	}
	pthread_mutex_unlock(&mutex);
	return instance;
}

unique_ptr<CBaseTask> CTaskFactory::createTask(int clientFd, int bussinessType, char* data, size_t length)
{
	switch (bussinessType)
	{
	case 1://登录
	{	
		unique_ptr<CLoginTask> task(new CLoginTask(clientFd, data, length));
		return task;
	}
	case 3://验证码
	{
		unique_ptr<SendCodeTask> task(new SendCodeTask(clientFd, data, length));
		return task;
	}
	case 5://注册
	{
		unique_ptr<RegisterTask> task(new RegisterTask(clientFd, data, length));
		return task;
	}
	case 7://入场请求
	{
		unique_ptr<CCarEntryTask> task(new CCarEntryTask(clientFd, data, length));
		return task;
	}
	case 9://出场请求
	{
		unique_ptr<CCarLeaveTask> task(new CCarLeaveTask(clientFd, data, length));
		return task;
	}
	case 11://放行请求
	{
		unique_ptr<CAgreeLeaveTask> task(new CAgreeLeaveTask(clientFd, data, length));
		return task;
	}
	case 13://上传录制视频信息请求
	{
		unique_ptr<CUploadVideoTask> task(new CUploadVideoTask(clientFd, data, length));
		return task;
	}
	case 15://获取服务器所存储的视频日期 
	{
		unique_ptr<CVideoDateTask> task(new CVideoDateTask(clientFd, data, length));
		return task;
	}
	case 17://获取视频播放列表请求体 
	{
		unique_ptr<CVideoListTask> task(new CVideoListTask(clientFd, data, length));
		return task;
	}
	case 19://上传视频播放信息
	{
		unique_ptr<CUploadPlayinfoTask> task(new CUploadPlayinfoTask(clientFd, data, length));
		return task;
	}
	case 21://车辆信息查询请求体
	{
		unique_ptr<CParkingInfoTask> task(new CParkingInfoTask(clientFd, data, length));
		return task;
	}
	case 23://文件上传
	{
		unique_ptr<CFileUploadTask> task(new CFileUploadTask(clientFd, data, length));
		return task;
	}
	case 25://文件上传确认包
	{
		unique_ptr<CFileCheckTask> task(new CFileCheckTask(clientFd, data, length));
		return task;
	}
	case 27://修改车牌请求
	{
		unique_ptr<CUpdateCarNumberTask> task(new CUpdateCarNumberTask(clientFd, data, length));
		return task;
	}
	case 29://心跳服务
	{
		unique_ptr<CHeartServerTask> task(new CHeartServerTask(clientFd, data, length));
		return task;
	}
	case 31://客户端下线
	{
		unique_ptr<COffLineTask> task(new COffLineTask(clientFd, data, length));
		return task;
	}
	case 2:// 登录返回
	case 4:// 验证码返回
	case 6:// 注册返回
	case 8:// 入场返回
	case 10://出场返回
	case 12://放行返回
	case 14://上传录制视频信息返回
	case 16://获取服务器所存储的视频日期
	case 18://获取视频播放列表
	case 20://上传播放视频信息返回体
	case 22://车辆信息查询
	case 24://文件上传-丢失的碎片返回包
	case 26://文件上传-结果返回包
	case 28://修改车牌返回
	case 30://心跳服务
	case 32://客户端下线
	{
		unique_ptr<CSendBackTask> task(new CSendBackTask(clientFd, data, length));
		return task;
	}
	default:
		cout << "CTaskFactory未知的case:" << bussinessType << endl;
		break;
	}
	return nullptr;
}

CTaskFactory::CTaskFactory()
{
	pthread_mutex_init(&mutex, NULL);
}
