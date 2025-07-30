#include "SendCodeTask.h"

SendCodeTask::SendCodeTask(int fd, char* data, size_t len)
	:CBaseTask(fd,data,len)
{
}

void SendCodeTask::work()
{
	cout << "SendCodeTask正在执行" << endl;
	if (this->dataLen <= 0) {
		cout << "请求体长度小于0，异常" << endl;
		return;
	}
	//数据解析
	HEAD head;
	CodeRequest request;
	memcpy(&head, taskData, sizeof(HEAD));
	memcpy(&request, taskData + sizeof(HEAD), head.bussinessLength);
	cout << "验证码请求-账号:" << request.account <<  endl;
	//账号有效性校验
	cout << "account=" << request.account << endl;
	std::regex tel_reg("^1[3456789]\\d{9}$");
	bool ret = std::regex_match(request.account, tel_reg);
	std::cout << (ret ? "account valid" : "account invalid") << std::endl;
	if (!ret) {
		return;
	}

	//随机生成验证码
	std::string code = CTools::generateCode(6);
	//记录验证码与手机号对应关系，然后发送短信
	DataManager::messageCodeMap[request.account] = code;
	int res = MessageCodeSender::getInstance().sendVerificationCode(request.account, code);

	//准备返回体
	HEAD backHead;
	CommonBack backBody;
	backHead.bussinessLength = sizeof(CommonBack);
	backHead.bussinessType = 4;
	if (res == 0) {
		backBody.flag = 1;
		sprintf(backBody.message, "code=%s", code.c_str());
		cout << "验证码短信发送成功code=" << code << endl;
	}
	else {
		backBody.flag = 0;
		sprintf(backBody.message, "错误码=%d", res);
		cout << "短信发送失败错误码="<< res << endl;
	}
	//准备写入缓冲区
	char buf[sizeof(HEAD) + sizeof(CommonBack)];
	//返回包计算crc校验码
	backHead.crc = CTools::crc32((uint8_t*)&backBody, sizeof(CommonBack));
	memcpy(buf, &backHead, sizeof(HEAD));
	memcpy(buf + sizeof(HEAD), &backBody, sizeof(backBody));
	if (write(clientFd, buf, sizeof(buf)) == -1) {
		perror("发送验证码返回体 write err:");
	}
	else {
		cout << "发送验证码返回体-发送成功" << endl;
	}
}
