#pragma once
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <vector>
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

	//URL编码
	static std::string urlEncode(const std::string& str);
	//URL解码
	static std::string urlDecode(const std::string& str);

	//生成验证码
	static std::string generateCode(int length);

	//递归的创建目录
	static bool createDirectoryRecursive(const std::string& path, mode_t mode = 0777);
private:
	CTools() = delete;
	~CTools() = delete;

	// 保留字符列表（RFC 3986）
	static const std::string SAFE_CHARS;
};

