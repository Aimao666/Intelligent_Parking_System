#pragma once
#include <list>
#include <pthread.h>
#include <queue>
#include "CBaseTask.h"
#include <algorithm>
#include <map>
#include <memory>
#include <iostream>
using namespace std;
//根据当前cpu硬件参数而定
#define MIN_THREAD_NUM 6
#define MAX_THREAD_NUM 24
class CThreadPool
{
public:
	//加锁，解锁二次封装
	void lock();
	void unlock();
	//等待，唤醒二次封装
	void wait();
	void wakeup();

	//从空闲列表移动到忙碌列表
	void moveToBusy(pthread_t tid);
	//从忙碌列表移动到空闲列表
	void moveToIdle(pthread_t tid);

	//检查是否需要销毁多余线程
	void checkDestroyCondition();
	//检查是否需要创建更多线程
	void checkCreateCondition();

	//添加任务到任务队列
	void pushTask(unique_ptr<CBaseTask> task);
	//取任务
	unique_ptr<CBaseTask> popTask();
	//判断任务是否为空
	bool queueIsEmpty();
	//线程执行函数
	static void* thread_function(void* arg);

	//获取单例
	static CThreadPool* getInstance();

	//设置最大线程数
	void setMaxThreadNum(int maxThreadNum);

	//打印线程池资源数
	void printThreadPoolResource();
private:
	//单例模式
	//入参为初始化线程数，该线程池最大最小线程数与此有关
	CThreadPool(int initNum = MIN_THREAD_NUM);
	~CThreadPool();
	static pthread_mutex_t createMutex;
	static CThreadPool* instance;


	//线程数
	int minThreadNum;
	int currentThreadNum;
	int maxThreadNum;

	//任务队列
	queue<unique_ptr<CBaseTask>> taskQueue;
	//忙碌列表
	list<pthread_t>busyList;
	//空闲列表
	list<pthread_t>idleList;
	//线程互斥量 锁任务队列
	pthread_mutex_t mutex;
	//线程条件变量，作用是让线程出现阻碍等待和唤醒的操作，允许多个线程在同一个条件变量上等待，可以单独唤醒等待队列队头也可广播唤醒
	pthread_cond_t cond;
	//让线程在任务队列为空，且当前线程数大于最小线程数时自动销毁的思路
	map<pthread_t, bool> shouldExitFlagMap;//线程退出标志位map，各自维护自己的标志

};

