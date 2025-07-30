#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
class CHostAddress
{
public:
	//默认自分配ip
	CHostAddress(unsigned short port, unsigned int ip = INADDR_ANY);
	~CHostAddress();

	int getLength() const;
	struct sockaddr* getAddr() const;
	struct sockaddr_in getAddr_in() const;
	unsigned short getPort() const;
	void setPort(unsigned short port);
	unsigned int getIp() const;
	void setIp(unsigned int ip);

private:
	unsigned short port;
	unsigned int ip;
	struct sockaddr_in addr;
	int length;
};

