#include "CSendBackTask.h"

CSendBackTask::CSendBackTask(int fd, char* data, size_t len)
	:CBaseTask(fd,data,len)
{
}
//给客户端发送返回包，前置直接发，不管内容，内容都在后置已经编辑好了
void CSendBackTask::work()
{
	cout << "CSendBackTask正在执行" << endl; 
	if (this->dataLen <= 0) {
		cout << "请求体长度小于0，异常" << endl;
		return;
	}
	try{
		HEAD headBack;
		memcpy(&headBack, this->taskData, sizeof(HEAD));
		headBack.crc = CTools::crc32((uint8_t*)(taskData + sizeof(HEAD)), headBack.bussinessLength);
		//记录业务数
		pthread_mutex_lock(&DataManager::mutex);
		switch (headBack.bussinessType)
		{
		case 2:
			//登录
			++DataManager::loginNum;
			break;
		case 6:
			//注册
			++DataManager::registerNum;
			break;
		case 8:
			//入场
			++DataManager::entryNum;
			break;
		case 10:
			//出场
			++DataManager::leaveNum;
			break;
		case 12:
			//放行
			++DataManager::agreeLeaveNum;
			break;

		case 14:
			//上传视频录制信息
			++DataManager::uploadVideoInfoNum;
			break;
		case 18:
			//获取视频列表
			++DataManager::getVideoListNum;
			break;
		case 20:
			//上传视频播放信息
			++DataManager::uploadVideoPlayRecordNum;
			break;
		case 22:
			//车辆信息列表
			++DataManager::getCarListNum;
			break;
		case 26:
			//文件上传成功时记录业务数+1
			++DataManager::uploadFileNum;
			break;
		default:
			break;
		}
		pthread_mutex_unlock(&DataManager::mutex);
		
		memcpy(this->taskData, &headBack, sizeof(HEAD));
		//发送给客户端
		int res = -1;
		if ((res = write(this->clientFd, this->taskData, this->dataLen)) == -1) {
			perror("CSendBackTask write err:");
			cout << "clientFd=" << this->clientFd << endl;
		}
		else {
			pthread_mutex_lock(&DataManager::mutex);
			++DataManager::sendPacket;
			pthread_mutex_unlock(&DataManager::mutex);
			cout << "CSendBackTask发送成功,业务类型:" << headBack.bussinessType << " (预计,实际)字节数：(" <<
				dataLen << "," << res << ")" << endl;
		}
		//打印实时日志
		cout << "++++++++++++++实时日志+++++++++++++++" << endl;
		cout << "有效连接数：" << DataManager::heartServiceMap.size() << endl;
		cout << "接收数据包：" << DataManager::rcvPacket << "	发送数据包：" << DataManager::sendPacket << endl;
		cout << "登录请求数：" << DataManager::loginNum << "	注册请求数：" << DataManager::registerNum << endl;
		cout << "上传视频信息请求数：" << DataManager::uploadVideoInfoNum << "	上传视频播放记录请求数：" << DataManager::uploadVideoPlayRecordNum << endl;
		cout << "获取用户视频列表请求数：" << DataManager::getVideoListNum << "	完成的文件上传业务数：" << DataManager::uploadFileNum << endl;
		cout << "入场请求数：" << DataManager::entryNum << "	出场请求数：" << DataManager::leaveNum << endl;
		cout << "放行请求数：" << DataManager::agreeLeaveNum << "	获取车辆信息列表请求数：" << DataManager::getCarListNum << endl;
		cout << "+++++++++++++++++++++++++++++++++++++" << endl;
	}
	catch (exception e) {
		cerr << "CSendBackTask exception:" << e.what() << endl;
	}

}
