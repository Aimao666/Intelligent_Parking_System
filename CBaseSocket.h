#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
using namespace std;
class CBaseSocket
{
public:
	//protocol=0时会默认根据前二者匹配好是udp还是tcp协议，如果要其他协议如icmp需要指定
	CBaseSocket(int type, int sin_family, int protocol = 0);
	virtual~CBaseSocket();
	int getSocketfd() const;
	void startConnect();
	void stopConnect();
	virtual int work() = 0;
protected:
	int socketfd;
	int type;
	int sin_family;
	int protocol;
};

