#pragma once

//接口类型：互亿无线触发短信接口，支持发送验证码短信、订单通知短信等。
// 账户注册：请通过该地址开通账户http://sms.ihuyi.com/register.html
// 注意事项：
//（1）调试期间，请用默认的模板进行测试，默认模板详见接口文档；
//（2）请使用APIID（查看APIID请登录用户中心->验证码短信->产品总览->APIID）及 APIkey来调用接口；
//（3）该代码仅供接入互亿无线短信接口参考使用，客户可根据实际需要自行编写；

// DEMO仅作参考
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>
#include "CTools.h"
#define SA struct sockaddr
#define MAXLINE 4096
#define MAXSUB  2000
#define MAXPARAM 2048
#define LISTENQ 1024
extern int h_errno;

class MessageCodeSender
{
public:
	/**
	* 发送短信
	*/
	static ssize_t send_sms(char* mobile) ;
private:
	MessageCodeSender();
	~MessageCodeSender();
	/**
	* 发http post请求
	*/
	static ssize_t http_post(char* page, char* poststr) ;
	static int socked_connect(char* arg) ;

private:
	static int basefd;
	static char* hostname;
	static char* send_sms_uri;
	//用户名是登录用户中心->验证码短信->产品总览->APIID
	static char* account;
	//查看密码请登录用户中心->验证码短信->产品总览->APIKEY
	static char* password;
};

