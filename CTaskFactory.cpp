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

	case 2:// 登录返回
	case 4:// 验证码返回
	case 6://注册返回
	case 8:
	case 10:
	case 12:
	case 14:
	case 16:
	case 18:
	case 20:
	case 22:
	case 24://文件上传-丢失的碎片返回包
	case 26://文件上传-结果返回包
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
