#pragma once
#include <sys/epoll.h>
#include "CTcpServer.h"
#include <string.h>
#include <memory>
#include "protocol.h"
#include "CThreadPool.h"
#include "CTaskFactory.h"
#include "CTools.h"
#include <fcntl.h>
#include <unordered_map>
#include "CReadShmTask.h"
using namespace std;
#define EPOLL_SIZE 5
#define DEFAULT_EVENTS_NUM 5
class CEpollServer
{
public:
	CEpollServer(unsigned short port, int maxEvents = DEFAULT_EVENTS_NUM);
	~CEpollServer();
	void work();
	CThreadPool* getPool() const;
	void closeClient(int clientFd);
	void handleClientData(int clientFd);
	void handleClientData2(int clientFd);
private:
	/*
	函数参数:
		@parm1:文件描述符fd，表示要操作的目标文件描述符,可以是套接字、管道、定时器、信号等
		@parm2:操作类型op，表示要对目标文件描述符进行什么操作，可以是EPOLL_CTL_ADD添加fd到epoll，EPOLL_CTL_MOD修改和EPOLL_CTL_DEL删除
		@parm3:epoll事件就绪时的触发类型，可以是1水平触发LT或者2边缘触发ET
	*/
	void epollControl(int fd, int op, int type);
	//搭建网络通道
	unique_ptr<CTcpServer>socketServer;
	//代表epoll红黑树头结点
	int epollfd;
	int socketfd;
	int maxEvents;//这个epoll对象处理的事件大致数量,与wait一次能清空的最大就绪队列事件个数相同
	//就绪队列发生事件的fd数量
	int epollWaitNum;
	//期望发生xx事件的结构体 其中包含fd和期望发生的事件
	struct epoll_event epollevent;

	//已经发生事件的结构体数组(其中包含已经发生事件的fd)---就绪列表
	unique_ptr<struct epoll_event[]>epolleventArray;

	unique_ptr<CThreadPool> pool;

	//任务工厂
	CTaskFactory* taskFctory;

	//单独的线程去监听消息队列的消息
	pthread_t msgrcvThread;
	//线程执行函数
	static void* msgrcvThread_function(void* arg);

	enum ConnectionState {
		READING_HEADER,   // 正在读取头部
		READING_BODY,     // 正在读取主体
		PROCESSING        // 已读取完整请求
	};

	//状态机
	struct ConnectionData {
		ConnectionState state;
		HEAD header;
		std::vector<char> bodyBuffer;//内存又vector管理，加上resize分配内存就避免了动态扩容开销大的风险
		size_t bytesRead;
	};

	//每一个客户端文件描述符fd对应一个状态机
	unordered_map<int, ConnectionData> connections;
};

