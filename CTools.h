#pragma once
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
using namespace std;
class CTools
{
public:
	/*
	函数功能：int转string
	函数参数：int num,int minlen 最小长度，即如果转化出来的string的长度小于该长度则需要在前面自动补0
	函数返回：string
	*/
	static string itos(int num, int minlen = 1);
	//CRC-32计算函数
	static uint32_t crc32(const uint8_t* data, size_t length);
private:
	CTools();
	~CTools();
	
};

