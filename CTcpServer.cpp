#include "CTcpServer.h"

CTcpServer::CTcpServer(unsigned short port, int type, int sin_family)
	:CBaseSocket(type,sin_family)
{
	hostAddress.reset(new CHostAddress(port));
}

CTcpServer::~CTcpServer()
{
}

int CTcpServer::work()
{
	if (socketfd == -1) {
		cout << "请先确保socket网络通道搭建成功" << endl;
		return 0;
	}
	//端口复用技术，解决address already is use端口被占用的异常问题
    //必须在bind函数前完成
	int opt_val = 1;
	setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (const void*)(&opt_val), sizeof(opt_val));
	//0成功-1失败
	if (bind(this->socketfd, hostAddress->getAddr(), hostAddress->getLength()) == -1) {
		perror("bind err ");
		return 0;
	}
	//初始化该网络通道能监听10个客户端，后续能通过其他手段修改个数
	if (listen(socketfd, 10) == -1) {
		perror("listen err");
		return 0;
	}
	cout << "服务器网络搭建成功" << endl;
	return 1;
}
