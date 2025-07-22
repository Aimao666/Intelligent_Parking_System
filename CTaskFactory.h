#pragma once
#include "CBaseTask.h"
#include <pthread.h>
#include <memory>
#include "CLoginTask.h"
using namespace std;
class CTaskFactory
{
public:
	~CTaskFactory();
	static CTaskFactory* getInstance();
	unique_ptr<CBaseTask> createTask(int clientFd, int bussinessType, char* data, size_t length);
private:
	CTaskFactory();
	static CTaskFactory* instance;
	static pthread_mutex_t mutex;

};

