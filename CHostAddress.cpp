#include "CHostAddress.h"

CHostAddress::CHostAddress(unsigned short port, unsigned int ip)
	:ip(ip),port(port)
{
	addr.sin_port = htons(port);//大小端转换
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	this->length = sizeof(this->addr);
}

CHostAddress::~CHostAddress()
{
}

int CHostAddress::getLength() const
{
	return length;
}

sockaddr* CHostAddress::getAddr() const
{
	return (sockaddr*)(&addr);
}

sockaddr_in CHostAddress::getAddr_in() const
{
	return addr;
}

unsigned short CHostAddress::getPort() const
{
	return port;
}

void CHostAddress::setPort(unsigned short port)
{
	this->port = port;
}

unsigned int CHostAddress::getIp() const
{
	return ip;
}

void CHostAddress::setIp(unsigned int ip)
{
	this->ip = ip;
}
