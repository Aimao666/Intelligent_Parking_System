#include "CEpollServer.h"

CEpollServer::CEpollServer(unsigned short port, int maxEvents)
{
	//搭建Tcp网络通道
	socketServer.reset(new CTcpServer(port));
	if (socketServer->work() == 0) {
		cout << "网络连接失败，请检查端口是否被占用" << endl;
		exit(0);
	}
	socketfd = socketServer->getSocketfd();
	epollfd = 0;
	this->maxEvents = maxEvents;
	//相当于初始化maxEvents个迎宾小姐，一次最多招待maxEvents人，但是多的人可以排队
	epolleventArray.reset(new struct epoll_event[maxEvents]);
	/*
	1. epoll_create(size) 的 size 参数
	历史遗留参数：
	在早期 Linux 内核（2.6.8 之前），size 表示 epoll 实例初始监控的文件描述符（fd）数量。
	但自 Linux 2.6.8 起，size 被忽略，内核会动态调整 epoll 的容量。

	现代用法：
	为了兼容性，size 只需传递一个大于 0 的值（如 5），实际无限制。
	*/
	//1.创建epoll红黑树结构，参数含义为可以识别到有多少个fd
	epollfd = epoll_create(EPOLL_SIZE);
	//2.将服务器网络通道socketfd保存到epoll监听事件列表中，一旦是socketfd事件被触发那么一定是有客户端来连接
	/*EPOLLIN
	当文件描述符（如套接字、管道等）的接收缓冲区中有数据可读时，epoll 会报告 EPOLLIN 事件。
	对于 TCP 套接字，以下情况会触发：
		客户端发送数据到达。
		客户端正常关闭连接（触发 read() 返回 0）。
	对于 监听套接字（Server Socket），表示有新连接到达（需调用 accept()）。
	*/

	//这里需要LT而不是ET，如果是ET就要用while然后就会卡死，如果不用while，就会监听不到同时到达的客户端
	epollControl(socketfd, EPOLL_CTL_ADD, 1);
	bzero(this->epolleventArray.get(), sizeof(this->epolleventArray.get()));
	bzero(&this->epollevent, sizeof(this->epollevent));
	//初始化线程池
	pool.reset(new CThreadPool(4));
	taskFctory = CTaskFactory::getInstance();
	pthread_create(&msgrcvThread, NULL, CEpollServer::msgrcvThread_function, this);
}

CEpollServer::~CEpollServer()
{
	//等待消息队列监听线程回收
	pthread_join(this->msgrcvThread, nullptr);
}

void CEpollServer::work()
{
	int acceptfd = 0;
	int res = 0;
	if (socketfd == -1) {
		cout << "网络连接失败" << endl;
		return;
	}
	while (1) {
		cout << "epoll wait ------ epoll开始等待事件发生....." << endl;
		//3.阻塞等待事件发生，一旦发生事件则从epoll红黑树中将fd取出，保存到epolleventArray中
		//返回值epollWaitNum 是真实取到的发生事件的fd的个数,maxEvents为数组最大长度，也就是最大能同时监听的事件数量
		epollWaitNum = epoll_wait(epollfd, epolleventArray.get(), maxEvents, -1);
		if (epollWaitNum < 0) {
			perror("epoll_wait err");
			return;
		}
		//4.利用循环遍历已经发生了事件的fd，对每一个fd进行相应的处理
		for (int i = 0; i < epollWaitNum; i++) {
			//如果是socketfd代表客户端来连接
			if (epolleventArray[i].data.fd == socketfd) {
				//acceptfd 文件描述符代表连接成功客户端
				//accept阻塞式函数一直等待客户端来连接
				acceptfd = accept(socketfd, NULL, NULL);
				if (acceptfd < 0) {
					perror("accept err");
					cout << "异常客户端fd，强制结束" << endl;
				}
				cout << "socketfd事件发生，服务器监听到客户端上线acceptfd=" << acceptfd << endl;
				//5.如果是客户端来连接那么，连接成功的acceptfd也要添加到epoll中保存
				// 设置非阻塞模式 - ET模式必须的!
				int flags = fcntl(acceptfd, F_GETFL, 0);
				fcntl(acceptfd, F_SETFL, flags | O_NONBLOCK);
				epollControl(acceptfd, EPOLL_CTL_ADD, 2);
				//记录上线心跳
				DataManager::ClientValue *clientValue = new DataManager::ClientValue{ "",CTools::getDatetime() };
				pthread_mutex_lock(&DataManager::mutex);
				DataManager::heartServiceMap[acceptfd] = unique_ptr<DataManager::ClientValue>(clientValue);
				pthread_mutex_unlock(&DataManager::mutex);
			}
			//客户端下线或者客户端发来报文
			else if (epolleventArray[i].events & EPOLLIN) {
				handleClientData(epolleventArray[i].data.fd);
			}
			else {
				cout << "预料之外的事件类型" << endl;
			}
		}
	}
}

CThreadPool* CEpollServer::getPool() const
{
	return this->pool.get();
}

void CEpollServer::closeClient(int clientFd)
{
	epollControl(clientFd, EPOLL_CTL_DEL, 2);
	close(clientFd);
	//connections.erase(clientFd);
	cout << "关闭客户端连接: " << clientFd << endl;
}

void CEpollServer::epollControl(int fd, int op, int type)
{
	bzero(&epollevent, sizeof(epollevent));
	epollevent.data.fd = fd;
	if (type == 1) {
		//水平触发LT
		epollevent.events = EPOLL_EVENTS::EPOLLIN;
	}
	else if (type == 2) {
		//边缘触发ET
		epollevent.events = EPOLL_EVENTS::EPOLLIN | EPOLL_EVENTS::EPOLLET;
	}
	epoll_ctl(epollfd, op, fd, &epollevent);
}
//线程执行函数
void* CEpollServer::msgrcvThread_function(void* arg)
{
	CEpollServer* server = static_cast<CEpollServer*>(arg);
	IPCManager* ipc = IPCManager::getInstance();
	int msgid = ipc->getMsgid();
	int semid = ipc->getSemid();
	int block_num = ipc->getNums_sems();//信号量数组大小，对应共享内存被拆分成多少块
	int shmid = ipc->getShmid();
	if (msgid < 0 || semid < 0 || shmid < 0) {
		cout << "ipc分配失败,shmid=" << shmid << " semid=" << semid << " msgid=" << msgid << endl;
		return nullptr;
	}
	//void* shmaddr = shmat(shmid, NULL, 0);//用于接收共享内存块起始地址
	//if (shmaddr == (void*)-1) {
	//	perror("shmat 失败");
	//	cout << "无法连接共享内存，程序退出" << endl;
	//	return nullptr;
	//}
	//int indexArr[block_num];//索引区
	while (true) {
		IPCManager::MSGBUF buffer;
		buffer.mtype = 2;
		if (msgrcv(msgid, &buffer, sizeof(buffer.mtext), buffer.mtype, 0) < 0) {
			perror("msgrcv err");
			continue;
		}
		cout << "============接收到消息队列消息:" << buffer.mtext << endl;
		int index = atoi(buffer.mtext);//知道索引区可写的下标
		// 检查索引范围
		if (index < 0 || index >= block_num) {
			cerr << "无效索引: " << index << endl;
			continue;
		}
		//创建读取共享内存的任务，让线程池线程执行读取共享内存的任务

		server->pool->pushTask(unique_ptr<CReadShmTask>(new CReadShmTask(index, server->pool.get())));
		//ipc->sem_p(semid, index);
		//memcpy(indexArr, shmaddr, sizeof(indexArr));
		////判断目标区域是否真的可读:0可写，1后置可读,2前置可读
		//if (indexArr[index] == 2) {
		//	cout << "目标区域可读" << endl;
		//	//拷贝数据
		//	char shmBuffer[MAX_BODY_LENGTH];//实际内存块
		//	memcpy(shmBuffer, (char*)shmaddr + sizeof(int) * block_num + index * MAX_BODY_LENGTH, MAX_BODY_LENGTH);
		//	HEAD head;
		//	memcpy(&head, shmBuffer, sizeof(HEAD));
		//	int bodyLen = head.bussinessLength;
		//	auto task = CTaskFactory::getInstance()->createTask(head.crc, head.bussinessType, shmBuffer, sizeof(HEAD) + head.bussinessLength);
		//	//任务给到线程池
		//	server->pool->pushTask(move(task));
		//}
		//ipc->sem_v(semid, index);
	}
	//shmdt(shmaddr);
	return nullptr;
}

//处理客户端下线或者客户端报文
void CEpollServer::handleClientData(int clientFd)
{
	//ET模式下要读该客户端缓冲区的所有内容!!!!!!!!必须循环读取直到EAGAIN
	bool connectionClosed = false;
	bool readError = false;
	int res = 0;
	while (true) {
		HEAD head = { 0 };
		//1.读请求头
		res = read(clientFd, &head, sizeof(HEAD));//后续可改sizeof(HEAD)
		if (res == sizeof(HEAD)) {
			cout << "res == sizeof(HEAD)=" << res << endl;
			cout << "head.bussinessLength=" << head.bussinessLength << endl;
			cout << "head.bussinessType=" << head.bussinessType << endl;
			cout << "head.crc=" << head.crc << endl;
			//++请求头合法性校验++
			if (head.bussinessLength > MAX_BODY_LENGTH || head.bussinessType > MAX_BUSINESS_TYPE || head.crc <= 0) {
				cout << "请求头数据合法性校验失败" << endl;
				//清除缓冲区
				cleanReadBuffer(clientFd);
				break;
			}

			//客户端发来请求
			char buf[head.bussinessLength];
			//2.读请求体
			res = read(clientFd, buf, head.bussinessLength);
			//3.验证请求体数据合法性
			if (res == head.bussinessLength && CTools::crc32((uint8_t*)buf, head.bussinessLength) == head.crc) {
				cout << "crc校验成功:" << head.crc << endl;
				//任务创建器,请求体给到任务中
				char headAndBody[sizeof(HEAD) + head.bussinessLength];
				memcpy(headAndBody, &head, sizeof(HEAD));
				memcpy(headAndBody + sizeof(HEAD), buf, head.bussinessLength);
				//cout << "Epoll:clientFd=" << clientFd << endl;
				auto task = taskFctory->createTask(clientFd, head.bussinessType, headAndBody, sizeof(headAndBody));
				//任务给到线程池
				if (task) {
					pool->pushTask(move(task));
				}
				//接收到合法包数量+1
				pthread_mutex_lock(&DataManager::mutex);
				++DataManager::rcvPacket;
				pthread_mutex_unlock(&DataManager::mutex);
			}
			else {
				//验证发现数据不合法1.读取的请求体长度小了 2.crc校验不通过
				if (res < head.bussinessLength) {
					//半包，清空缓冲区
					cout << "半包res=" << res << "<head.bussinessLength=" << head.bussinessLength << endl;
					cleanReadBuffer(clientFd);
					break;
				}
				cout << "crc校验失败head.crc" << head.crc << "!=" << CTools::crc32((uint8_t*)buf, head.bussinessLength) << endl;
			}
		}
		else if (res == 0) {
			// 客户端关闭连接
			cout << "res == 0客户端关闭连接" << endl;
			connectionClosed = true;

			break;
		}
		else if (res == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
			// EAGAIN或EWOULDBLOCK：非阻塞模式下暂时无数据，下次再试
			// ET模式：暂时无数据，等待下次事件
			cout << "res == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)" << endl;
			break;
		}
		else if (res > 0) {
			//半包，清空缓冲区
			cout << "请求体半包res=" << res << endl;
			cleanReadBuffer(clientFd);
			break;
		}
		else {
			// 其他错误
			cout << "其他错误,res=" << res << " errno=" << errno << endl;
			readError = true;
			break;
		}
		// 检查是否还有数据（ET模式可能需要处理多个请求）
		if (connectionClosed || readError) break;
	}
	// 处理连接关闭或错误
	if (connectionClosed || readError) {
		//没读到东西，客户端下线
		closeClient(clientFd);
	}
}

//清空目标客户端fd的缓冲区
void CEpollServer::cleanReadBuffer(int clientFd)
{
	char buf[5000];
	int res = 1;
	while (res > 0) {
		res = read(clientFd, buf, sizeof(buf));
	}
}

int CEpollServer::getSocketfd()
{
	return socketfd;
}

