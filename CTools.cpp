#include "CTools.h"

CTools::CTools()
{
}

CTools::~CTools()
{
}
/*
函数功能：int转string
函数参数：int num,int minlen 最小长度，即如果转化出来的string的长度小于该长度则需要在前面自动补0
函数返回：string
*/
string CTools::itos(int num, int minlen) {
	stringstream ss;
	ss << num;
	string res = ss.str();
	if (res.size() < minlen)
	{
		res.insert(0, minlen - res.size(), '0');
	}

	return res;
	//写法2	
	//	//使用 std::setw 和 std::setfill 自动补零
	//    ss << std::setw(minlen) << std::setfill('0') << num;
	//    return ss.str();
}

uint32_t CTools::crc32(const uint8_t* data, size_t length)
{
	static const uint32_t polynomial = 0xEDB88320;
	uint32_t crc = 0xFFFFFFFF;
	for (size_t i = 0; i < length; ++i) {
		uint8_t byte = data[i];
		crc ^= byte;
		for (int j = 0; j < 8; ++j) {
			if (crc & 1) {
				crc = (crc >> 1) ^ polynomial;
			}
			else {
				crc >> 1;
			}
		}

	}
	return crc ^ 0xFFFFFFFF;
}
