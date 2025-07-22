#pragma once
#include "CBaseSocket.h"
#include "CHostAddress.h"
#include <memory>
using namespace std;
class CTcpServer
	:public CBaseSocket
{
public:
	CTcpServer(unsigned short port, int type = SOCK_STREAM, int sin_family = AF_INET);
	~CTcpServer();
	int work();
private:
	unique_ptr<CHostAddress>hostAddress;
};

