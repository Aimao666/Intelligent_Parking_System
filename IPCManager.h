#pragma once
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <iostream>
#include <pthread.h>
using namespace std;
class IPCManager
{
public:
	union semun {
		int              val;    /* Value for SETVAL */
		struct semid_ds* buf;    /* Buffer for IPC_STAT, IPC_SET */
		unsigned short* array;  /* Array for GETALL, SETALL */
		struct seminfo* __buf;  /* Buffer for IPC_INFO
									(Linux-specific) */
	};

	typedef struct msgbuffer {
		long mtype;
		char mtext[200];
	}MSGBUF;
	~IPCManager();
	//创建共享内存
	int initShm(key_t key, size_t size);
	//创建消息队列
	int initMsg(key_t key);

	//创建信号量+赋值，IPC_EXCL避免后置服务器调用时覆盖前置对信号量的赋值
	int initSem(key_t key, int nsems, int value);
	/*
	信号量初始化 
		semid为信号量id,不是key
		sem_num对应信号量数组下标，表示要初始化哪个信号量
		value这个信号量有多少个资源
	*/
	int sem_setVal(int semid, int sem_num, int value);

	//信号量P操作 -1 参数semid:对应信号量id 参数sem_index:你要做-1操作的信号量数组下标元素
	void sem_p(int semid, int sem_index);
	//信号量V操作 +1 参数semid:对应信号量id 参数sem_index:你要做-1操作的信号量数组下标元素
	void sem_v(int semid, int sem_index);

	int getNums_sems()const;

	int getSemid()const;
	key_t getSemkey()const;

	int getShmid()const;
	key_t getShmkey()const;

	int getMsgid()const;
	key_t getMsgkey()const;

	//单例设计模式
	static IPCManager* getInstance();
private:
	IPCManager();
	static IPCManager* instance;
	static pthread_mutex_t mutex;
	key_t semkey;//信号量key
	int semid;//信号量id
	int nums_sems;//信号量数组长度

	key_t shmkey;//共享内存key
	int shmid;//共享内存id

	key_t msgkey;//消息队列key
	int msgid;//消息队列id

};

