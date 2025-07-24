#include "CBaseTask.h"

CBaseTask::CBaseTask(int fd, char* data, size_t len)
    : clientFd(fd), dataLen(len) {
    if (len > 0) {
        taskData = new char[len];
        memcpy(taskData, data, len);
    }
}
CBaseTask::~CBaseTask()
{
    if (dataLen > 0) {
        delete[] taskData;
    }
}

//保存数据到共享内存
void CBaseTask::saveData() {

	//数据放到共享内存
	IPCManager* ipc = IPCManager::getInstance();
	int msgid = ipc->getMsgid();
	int semid = ipc->getSemid();
	int shmid = ipc->getShmid();
	cout << "msgid=" << msgid << " semid=" << semid << " shmid=" << shmid << endl;
	int block_num = ipc->getNums_sems();//信号量数组大小，对应共享内存被拆分成多少块
	void* shmaddr = shmat(shmid, NULL, 0);
	if (shmaddr == (void*)-1) {
		perror("shmat 失败");
		cout << "无法连接共享内存，任务退出" << endl;
		return ;
	}
	int indexArr[block_num];//索引区
	int targetIndex = -1;
	//索引区上锁，遍历索引区找到可写的索引:0可写，1后置可读，2前置可读
	//cout << "准备获取索引区全局信号量IPCManager::INDEX_LOCK_SEM="<< IPCManager::INDEX_LOCK_SEM << endl;
	ipc->sem_p(semid, IPCManager::INDEX_LOCK_SEM);
	//cout << "获取索引区全局信号量" << endl;
	memcpy(indexArr, shmaddr, sizeof(indexArr));
	for (int i = 0; i < block_num; ++i) {
		if (indexArr[i] == 0) {
			targetIndex = i;
			//cout << "遍历索引区，找到可写索引" << targetIndex << endl;
			//将目标索引 的值改为后置可读，注意虽然后置可读，但是因为没发消息队列所以理论上后置还不可能读这里
			indexArr[i] = 1;
			memcpy((char*)shmaddr + sizeof(int) * i, &indexArr[i],  sizeof(int));
			break;
		}
	}
	ipc->sem_v(semid, IPCManager::INDEX_LOCK_SEM);
	if (targetIndex >= 0) {
		//找到可写区域，锁定目标块
		ipc->sem_p(semid, targetIndex);
		//cout << "内存拷贝:"<< targetIndex << endl;
		//拷贝请求头+体到共享内存
		memcpy((char*)shmaddr + sizeof(int) * block_num + targetIndex * ipc->getSingleBlockSize(), this->taskData, this->dataLen);
		ipc->sem_v(semid, targetIndex);
		IPCManager::MSGBUF msgbuf;
		msgbuf.mtype = 1;
		sprintf(msgbuf.mtext, "%d", targetIndex);
		//发消息队列通知后置可读
		if (msgsnd(msgid, &msgbuf, sizeof(msgbuf.mtext), 0) == -1) {
			perror("msgsnd err");

		}
		else
		{
			cout << "消息队列发送消息成功 targetIndex =" << targetIndex << endl;
		}
	}
	else {
		cout << "未找到可写区域" << endl;
	}
	shmdt(shmaddr);

	////遍历索引区
	//for (int i = 0; i < block_num; ++i) {
	//	ipc->sem_p(semid, i);
	//	memcpy(indexArr + i, shmaddr + sizeof(int) * i, sizeof(int));
	//	//判断该索引区是否可写:0可写，1后置可读，2前置可读
	//	if (indexArr[i] == 0) {
	//		indexArr[i] = 1;
	//		//索引区设置为可读
	//		memcpy((char*)shmaddr + sizeof(int) * i, indexArr + i, sizeof(int));
	//		//拷贝请求头+体到共享内存
	//		memcpy((char*)shmaddr + sizeof(indexArr) + i * ipc->getSingleBlockSize(), this->taskData, this->dataLen);
	//		ipc->sem_v(semid, i);
	//		IPCManager::MSGBUF msgbuf;
	//		msgbuf.mtype = 1;
	//		sprintf(msgbuf.mtext, "%d", i);
	//		//发消息队列通知后置可读
	//		if (msgsnd(msgid, &msgbuf, sizeof(msgbuf.mtext), 0) == -1) {
	//			perror("msgsnd err");

	//		}
	//		else
	//		{
	//			cout << "消息队列发送消息成功 index =" << i << endl;
	//		}
	//		//业务完成跳出循环
	//		break;
	//	}
	//	ipc->sem_v(semid, i);
	//}
	//shmdt(shmaddr);
}
int CBaseTask::getClientFd() const
{
    return clientFd;
}

char* CBaseTask::getTaskData() const
{
    return taskData;
}

size_t CBaseTask::getDataLen() const
{
    return dataLen;
}

