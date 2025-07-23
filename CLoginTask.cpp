#include "CLoginTask.h"

CLoginTask::CLoginTask(int fd, char* data, size_t len)
	:CBaseTask(fd,data,len)
{
}

CLoginTask::~CLoginTask()
{
}

void CLoginTask::work()
{
	cout << this->taskData << "正在执行" << endl;
	if (this->dataLen <= 0) {
		cout << "请求体长度小于0，异常" << endl;
		return;
	}
	//数据解析
	HEAD head;
	LoginRequest loginRequest;
	memcpy(&head, taskData, sizeof(HEAD));
	memcpy(&loginRequest, taskData + sizeof(HEAD), head.bussinessLength);
	cout << "登录请求-账号:" << loginRequest.account <<"	密码:" << loginRequest.password << endl;
	//数据放到共享内存
	IPCManager* ipc = IPCManager::getInstance();
	int msgid = ipc->getMsgid();
	int semid = ipc->getSemid();
	int shmid = ipc->getShmid();
	int block_num = ipc->getNums_sems();//信号量数组大小，对应共享内存被拆分成多少块
	void* shmaddr = shmat(shmid, NULL, 0);
	int indexArr[block_num];//索引区
	memcpy(indexArr, shmaddr, sizeof(indexArr));
	//遍历索引区
	for (int i = 0; i < block_num; ++i) {
		ipc->sem_p(semid, i);
		//判断该索引区是否可写:0可写，1可读
		if (indexArr[i] == 0) {
			indexArr[i] = 1;
			memcpy((char*)shmaddr + sizeof(indexArr) + i * ipc->getSingleBlockSize(), this->taskData, this->dataLen);
			//索引区设置为可读
			memcpy((char*)shmaddr + sizeof(int) * i, indexArr + i, sizeof(int));
			ipc->sem_v(semid, i);
			IPCManager::MSGBUF msgbuf;
			msgbuf.mtype = 1;
			sprintf(msgbuf.mtext, "%d", i);
			//发消息队列通知后置可读
			if (msgsnd(msgid, &msgbuf, sizeof(msgbuf.mtext), 0) == -1) {
				perror("msgsnd err");

			}
			else
			{
				cout << "消息队列发送消息成功 index =" << i << endl;
			}
			//业务完成跳出循环
			break;
		}
		ipc->sem_v(semid, i);
	}
}
