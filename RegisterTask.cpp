#include "RegisterTask.h"

RegisterTask::RegisterTask(int fd, char* data, size_t len) 
	:CBaseTask(fd, data, len)
{
}

void RegisterTask::work()
{
	cout << "RegisterTask正在执行" << endl;
	if (this->dataLen <= 0) {
		cout << "dataLen数据长度小于0，异常" << endl;
		return;
	}
	//数据解析
	HEAD head;
	RegisterRequest request;
	memcpy(&head, taskData, sizeof(HEAD));
	memcpy(&request, taskData + sizeof(HEAD), head.bussinessLength);
	cout << "注册请求-账号:" << request.account << "	验证码:" << request.code << endl;
	//账号有效性校验
	cout << "account=" << request.account << endl;
	std::regex tel_reg("^1[3456789]\\d{9}$");
	bool ret = std::regex_match(request.account, tel_reg);
	std::cout << (ret ? "account valid" : "account invalid") << std::endl;
	if (!ret) {
		return;
	}
	
	//校验手机号与验证码是否一致
	auto it = DataManager::messageCodeMap.find(request.account);
	if (it != DataManager::messageCodeMap.end() ) {
		cout << "服务器存储的手机号和验证码信息为" << it->first << ":" << it->second << endl;
		//校验一致通过
		//数据放到共享内存
		if (it->second == request.code) {
			//拿crc码存一下fd，因为进来的时候已经校验过了，这个字段暂时没啥用
			head.crc = this->clientFd;
			//head修改了，需要保存到缓冲区，然后写入共享内存
			memcpy(taskData, &head, sizeof(HEAD));
			IPCManager::getInstance()->saveData(this->taskData,this->dataLen,1);
		}
	}
	else {
		//发返回体告知验证码不对
		HEAD backHead;
		CommonBack backBody;
		backHead.bussinessLength = sizeof(CommonBack);
		backHead.bussinessType = 6;
		backBody.flag = 0;
		sprintf(backBody.message, "%s%s", request.account,"验证码校验不通过");
		char buf[sizeof(HEAD) + sizeof(CommonBack)];
		//返回包计算crc校验码
		backHead.crc = CTools::crc32((uint8_t*)&backBody, sizeof(CommonBack));
		memcpy(buf, &backHead, sizeof(HEAD));
		memcpy(buf + sizeof(HEAD), &backBody, sizeof(CommonBack));
		//校验不通过，未知原因，因此打印一下手机号-验证码map
		cout << "手机号：验证码" << endl;
		for (auto pair : DataManager::messageCodeMap) {
			cout << pair.first << ":" << pair.second << endl;
		}
		if (write(clientFd, buf, sizeof(buf)) == -1) {
			perror("验证码检验失败 write err:");
		}
		else {
			cout << "注册返回体-验证码检验失败-发送成功" << endl;
		}
	}
}
