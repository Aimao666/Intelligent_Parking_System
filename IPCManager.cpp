#include "IPCManager.h"

int IPCManager::INDEX_LOCK_SEM ;
IPCManager* IPCManager::instance = nullptr;
pthread_mutex_t IPCManager::mutex;
IPCManager* IPCManager::getInstance()
{
    pthread_mutex_lock(&mutex);
    if (instance == nullptr) {
        instance = new IPCManager();
    }
    pthread_mutex_unlock(&mutex);
    return instance;
}

IPCManager::IPCManager()
{
    this->nums_sems = -1;
    this->semkey = -1;
    this->semid = -1;
    this->shmkey = -1;
    this->shmid = -1;
    this->msgkey = -1;
    this->msgid = -1;
    pthread_mutex_init(&mutex, NULL);
}

IPCManager::~IPCManager()
{
    if (shmid > 0) {
        shmctl(shmid, IPC_RMID, 0);
    }
    if (msgid > 0) {
        msgctl(msgid, IPC_RMID, 0);
    }
    if (semid > 0) {
        for (int i = 0; i < nums_sems; i++) {
            semctl(semid, i, IPC_RMID);//信号量删除
        }

    }
}

int IPCManager::initShm(key_t key, size_t size)
{
    shmkey = key;
    shmid = shmget(key, size, IPC_CREAT | IPC_EXCL | 0777);
    blockSize = size;
    singleBlockSize = (size - sizeof(int) * nums_sems) / nums_sems;
    if (shmid >= 0) {
        // 创建成功：当前进程是第一个创建
        cout << "消息队列创建成功" << endl;
    }
    else if (errno == EEXIST) {
        perror("shmget err");
        shmid = shmget(key, size, 0777);
        cout << "共享内存已存在，直接使用" << endl;
    }
    else {
        perror("shmget err:");
        return -1;
    }
    return shmid;
}

int IPCManager::initMsg(key_t key)
{
    msgkey = key;
    msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0777);
    if (msgid >= 0) {
        // 创建成功：当前进程是第一个创建
        cout << "消息队列创建成功" << endl;
    }
    else if (errno == EEXIST) {
        perror("msgget err");
        msgid = msgget(key, 0777);
        cout << "消息队列已存在，直接使用" << endl;
    }
    else {
        perror("msgget err:");
        return -1;
    }
    return msgid;
}

//创建信号量+赋值，IPC_EXCL避免后置服务器调用时覆盖前置对信号量的赋值
int IPCManager::initSem(key_t key, int nsems, int value)
{
    //信号量创建 key就是创建信号key，nums_sems是信号量数组长度，常用1，长度大于1表示有多个不同的信号量,IPC_EXCL- 排他
    //返回值-1表示失败，>0表示semid
    this->semkey = key;
    this->nums_sems = nsems;
    //最后一个信号量是代表所有索引区的信号量，前面的信号量对应内存块区
    this->semid = semget(key, nsems + 1, IPC_CREAT | IPC_EXCL | 0777);
    INDEX_LOCK_SEM = nsems;//全局索引信号量索引，下标为逻辑内存块个数
    if (semid >= 0) {
        // 创建成功：当前进程是第一个初始化者
        for (int i = 0; i < nsems + 1; i++) {
            sem_setVal(semid, i, 1); // 安全初始化
        }
        cout << "信号量创建成功" << endl;
    }
    else if (errno == EEXIST) {
        // 信号量已存在：直接获取其ID
        semid = semget(key, nsems, 0666);
        cout << "信号量已存在，直接使用" << endl;
    }
    else {
        perror("semget err:");
        return -1;
    }

    return semid;
}
/*
    信号量初始化
        semid为信号量id,不是key
        sem_num对应信号量数组下标，表示要初始化哪个信号量
        value这个信号量有多少个资源
    */
int IPCManager::sem_setVal(int semid, int sem_num, int value)
{
    //准备数据
    union semun arg;
    arg.val = value;
    int res = semctl(semid, sem_num, SETVAL, arg);
    if (res < 0) {
        perror("semctl err:");
        return 0;
    }
    else {
        cout << "信号量初始化赋值成功" << endl;
    }
    return res;
}

//信号量P操作 -1 参数semid:对应信号量id 参数sem_index:你要做-1操作的信号量数组下标元素
void IPCManager::sem_p(int semid, int sem_index)
{
    struct sembuf buf = { sem_index,-1,SEM_UNDO };
    int res = semop(semid, &buf, 1);
    if (res < 0) {
        perror("semop err");
        return;
    }
    cout << "信号量P 加锁成功" << endl;
}

//信号量V操作 +1 参数semid:对应信号量id 参数sem_index:你要做+1操作的信号量数组下标元素
void IPCManager::sem_v(int semid, int sem_index)
{
    struct sembuf buf = { sem_index,1,SEM_UNDO };
    int res = semop(semid, &buf, 1);
    if (res < 0) {
        perror("semop err");
        return;
    }
    cout << "信号量V 解锁成功" << endl;
}

int IPCManager::getSemid() const
{
    return semid;
}

key_t IPCManager::getSemkey() const
{
    return semkey;
}

int IPCManager::getShmid() const
{
    return shmid;
}

key_t IPCManager::getShmkey() const
{
    return shmkey;
}

int IPCManager::getMsgid() const
{
    return msgid;
}

key_t IPCManager::getMsgkey() const
{
    return msgkey;
}

int IPCManager::getSingleBlockSize() const
{
    return singleBlockSize;
}

int IPCManager::getBlockSize() const
{
    return blockSize;
}

int IPCManager::getNums_sems() const
{
    return nums_sems;
}
