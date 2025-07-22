#include "CBaseSocket.h"

CBaseSocket::CBaseSocket(int type, int sin_family, int protocol)
{
	this->sin_family = sin_family;
	this->type = type;
	this->protocol = protocol;
	startConnect();
}

CBaseSocket::~CBaseSocket()
{
}

int CBaseSocket::getSocketfd() const
{
	return socketfd;
}

void CBaseSocket::startConnect()
{
	this->socketfd = socket(sin_family, type, protocol); 
	if (socketfd == -1) {
		perror("socket err");
		return;
	}
	cout << "服务器网络初始化成功,socketid=" << socketfd << endl;
}

void CBaseSocket::stopConnect()
{
	//socketfd算是一种文件描述符，因此使用文件的关闭close就行
	if (socketfd != -1) {
		close(socketfd);
	}
	cout << "断开网络连接" << endl;
}

