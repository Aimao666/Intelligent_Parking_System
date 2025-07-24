#include "CTaskFactory.h"
CTaskFactory* CTaskFactory::instance = nullptr;
pthread_mutex_t CTaskFactory::mutex;
CTaskFactory::~CTaskFactory()
{
}

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
	default:
		break;
	}
	return nullptr;
}

CTaskFactory::CTaskFactory()
{
	pthread_mutex_init(&mutex, NULL);
}
