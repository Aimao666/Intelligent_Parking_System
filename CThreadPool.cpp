#include "CThreadPool.h"

CThreadPool::CThreadPool(int initNum)
{
	if (initNum > 0 && initNum * 4 <= MAX_THREAD_NUM) {
		this->minThreadNum = initNum;
		this->maxThreadNum = initNum * 4;
	}
	else {
		this->minThreadNum = MIN_THREAD_NUM;
		this->maxThreadNum = MAX_THREAD_NUM;
	}
	//线程互斥量初始化
	pthread_mutex_init(&mutex, NULL);
	//线程条件变量初始化
	pthread_cond_init(&cond, NULL);

	//初始化一定数量的线程
	for (currentThreadNum = 0; currentThreadNum < minThreadNum; ++currentThreadNum) {
		pthread_t tid;
		pthread_create(&tid, NULL, CThreadPool::thread_function, this);
		//添加到空闲列表
		idleList.push_back(tid);
		//是否需要退出标记为否
		this->shouldExitFlagMap.insert(make_pair(tid, false));
	}
}

CThreadPool::~CThreadPool()
{ 
	// 1. 设置所有线程的退出标志
	lock();
	for (auto& pair : shouldExitFlagMap) {
		pair.second = true; // 标记所有线程需要退出
	}
	// 2. 唤醒所有等待的线程
	pthread_cond_broadcast(&cond); // 广播唤醒所有线程
	unlock();
	// 3. 等待所有线程退出
	for (auto pair : shouldExitFlagMap) {
		pthread_join(pair.first, nullptr);
	}
	// 4. 清理资源
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	shouldExitFlagMap.clear();
	while (!taskQueue.empty()) {
		taskQueue.pop();
	}
}

void CThreadPool::lock()
{
	pthread_mutex_lock(&mutex);
}

void CThreadPool::unlock()
{
	pthread_mutex_unlock(&mutex);
}

void CThreadPool::wait()
{
	pthread_cond_wait(&cond, &mutex);
}

void CThreadPool::wakeup()
{
	pthread_cond_signal(&cond);
}

void CThreadPool::moveToBusy(pthread_t tid)
{
	list<pthread_t>::iterator iter;
	//在空闲链表查找tid对应的线程
	iter = find(this->idleList.begin(), this->idleList.end(), tid);
	if (iter != this->idleList.end()) {
		//从空闲链表中移除，添加忙碌链表
		this->idleList.erase(iter);
		this->busyList.push_back(*iter);
	}
}

void CThreadPool::moveToIdle(pthread_t tid)
{
	list<pthread_t>::iterator iter;
	//在忙碌链表查找tid对应的线程
	iter = find(this->busyList.begin(), this->busyList.end(), tid);
	if (iter != this->busyList.end()) {
		//从忙碌链表中移除，添加空闲链表
		this->busyList.erase(iter);
		this->idleList.push_back(*iter);
	}
}

void CThreadPool::checkDestroyCondition()
{
	//检查是否需要移除多余线程
	//任务队列为空&&当前线程数>最小线程数，此时将标志位设置为true，线程会在下轮执行时发现轮到自己销毁
	if (this->taskQueue.empty() && currentThreadNum > minThreadNum) {
		cout << "线程销毁tid=" << pthread_self() << endl;
		this->shouldExitFlagMap[pthread_self()] = true;
	}
}

void CThreadPool::checkCreateCondition()
{
	// 1. 计算关键指标
	int totalThreads = currentThreadNum;
	int pendingTasks = taskQueue.size();
	int available = maxThreadNum - totalThreads;

	// 2. 核心创建条件
	bool needCreate = false;

	// 条件1：任务积压
	if (pendingTasks > totalThreads * 1.2) {
		needCreate = true;
	}
	// 条件2：全忙且有余量
	else if (idleList.empty() && available > 0) {
		needCreate = true;
	}
	// 条件3：高负载比例
	else if (busyList.size() > idleList.size() * 3 && available > 0) {
		needCreate = true;
	}

	// 3. 创建线程
	if (needCreate) {
		// 计算创建数量：至少1个，最多3个或可用量
		int toCreate = min(available, max(1, min(3, pendingTasks / 5)));

		for (int i = 0; i < toCreate; i++) {
			pthread_t tid;
			if (pthread_create(&tid, NULL, thread_function, this) == 0) {
				++currentThreadNum;
				idleList.push_back(tid);
				shouldExitFlagMap[tid] = false;
				cout << "创建新线程: " << tid << endl;
			}
		}
	}
}

void CThreadPool::pushTask(unique_ptr<CBaseTask> task)
{
	lock();
	// 新任务添加到任务队列
	this->taskQueue.push(move(task));// 转移所有权
	// 如果有空闲线程就叫他起来干活
	// 检查空闲线程（在锁内）
	bool hasIdleThread = !idleList.empty();
	if (hasIdleThread) {
		cout << "线程唤醒" << endl;
		this->wakeup();  // 在锁内唤醒
	}
	checkCreateCondition();
	unlock();
}

unique_ptr<CBaseTask> CThreadPool::popTask()
{
	unique_ptr<CBaseTask> first = std::move(this->taskQueue.front());
	this->taskQueue.pop();
	return first;//返回值一般被视为右值，走的是移动构造，没有移动构造才会走拷贝构造，不需要显式的使用move转为右值引用
}

bool CThreadPool::queueIsEmpty()
{
	return this->taskQueue.empty();
}

void* CThreadPool::thread_function(void* arg)
{
	CThreadPool* pool = static_cast<CThreadPool*>(arg);
	//线程执行期间获取线程自己的id
	pthread_t tid = pthread_self();
	while (1) {
		pool->lock();
		//检查线程是否需要退出
		if (pool->shouldExitFlagMap[tid]) {
			pool->idleList.remove(tid);
			pool->shouldExitFlagMap.erase(tid);
			--pool->currentThreadNum;
			pool->unlock();
			pthread_exit(nullptr);  // 安全退出，后续代码不再执行
		}

		while (pool->queueIsEmpty() && !pool->shouldExitFlagMap[tid]) {
			//让线程进入空闲链表之后处于阻塞 因为现在任务队列为空
			cout << "当前无任务线程阻塞id=" << tid << endl;
			pool->wait();
		}

		/*cout << "-----------------------------" << endl;
		cout << "线程工作前 任务数：" << pool->taskQueue.size() << endl;
		cout << "线程工作前 忙碌链表线程数：" << pool->busyList.size() << endl;
		cout << "线程工作前 空闲链表线程数：" << pool->idleList.size() << endl;
		cout << "-----------------------------" << endl;*/

		//当前线程被唤醒了，且有任务能做且锁在自己手上
		unique_ptr<CBaseTask> task = pool->popTask();
		//移动到忙碌列表
		pool->moveToBusy(tid);
		pool->unlock();
		//处理任务

		try {
			task->work(); // 捕获任务执行中的异常
		}
		catch (std::exception& e) {
			std::cerr << "任务执行异常: " << e.what() << std::endl;
		}

		pool->lock();
		//移动到空闲链表
		pool->moveToIdle(tid);
		//检查当前线程是否多余了
		pool->checkDestroyCondition();
		pool->unlock();

		/*cout << "-----------------------------" << endl;
		cout << "线程工作后 任务数：" << pool->taskQueue.size() << endl;
		cout << "线程工作后 忙碌链表线程数：" << pool->busyList.size() << endl;
		cout << "线程工作后 空闲链表线程数：" << pool->idleList.size() << endl;
		cout << "-----------------------------" << endl;*/
	}

	return nullptr;
}
