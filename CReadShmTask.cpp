#include "CReadShmTask.h"

CReadShmTask::CReadShmTask(int shmIndex, CThreadPool* pool)
    :CBaseTask(shmIndex), pool(pool)
{
}

void CReadShmTask::work()
{
    cout << "CReadShmTask正在执行" << endl;
    IPCManager* ipc = IPCManager::getInstance();
    int block_num = ipc->getNums_sems();//信号量数组大小，对应共享内存被拆分成多少块
    size_t shm_size = block_num * (sizeof(int) + MAX_BODY_LENGTH);
    int shmid = ipc->getShmid();
    int semid = ipc->getSemid();
    void* shmaddr = shmat(shmid, NULL, 0);//用于接收共享内存块起始地址
    if (shmaddr == (void*)-1) {
        perror("shmat 失败");
        cout << "无法连接共享内存，程序退出,shmid=" << shmid << endl;
        return;
    }
    int indexArr[block_num];//索引区
    int index = this->shmIndex;
    //获取索引区信号量
    ipc->sem_p(semid, IPCManager::INDEX_LOCK_SEM);
    memcpy(indexArr, shmaddr, sizeof(indexArr));
    ipc->sem_v(semid, IPCManager::INDEX_LOCK_SEM);
    //判断目标区域是否真的可读:0可写，1后置可读,2前置可读
    if (indexArr[index] == 2) {
        cout << "目标区域可读" << endl;
        //拷贝数据
        char shmBuffer[MAX_BODY_LENGTH];//实际内存块
        //读取目标共享内存
        ipc->sem_p(semid, index);
        memcpy(shmBuffer, (char*)shmaddr + sizeof(int) * block_num + index * MAX_BODY_LENGTH, MAX_BODY_LENGTH);
        ipc->sem_v(semid, index);
        //目标共享内存索引区得修改为可写
        indexArr[index] = 0;
        ipc->sem_p(semid, IPCManager::INDEX_LOCK_SEM);
        memcpy((char*)shmaddr + sizeof(int) * index, indexArr + index, sizeof(int));
        ipc->sem_v(semid, IPCManager::INDEX_LOCK_SEM);
        HEAD head;
        memcpy(&head, shmBuffer, sizeof(HEAD));
        auto task = CTaskFactory::getInstance()->createTask(head.crc, head.bussinessType, shmBuffer, sizeof(HEAD) + head.bussinessLength);
        //任务给到线程池
        if (task) {
            pool->pushTask(move(task));
        }
        else {
            cout << "无法创建出目标任务,head.bussinessType=" << head.bussinessType << endl;
        }
    }
    else {
        cout << "目标区域index=" << index << ",区域索引值indexArr[index]=" << indexArr[index] << endl;
    }
    //断开与共享内存的连接
    shmdt(shmaddr);
    //sleep(3);
}
