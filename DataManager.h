#pragma once
#include <map>
#include <list>
#include <vector>
using namespace std;
class DataManager
{
public:
	//手机号,验证码映射表
	static map<string, string> messageCodeMap;
	
	//当前客户端连接
	static list<int> fdList;
	static int rcvPacket;//接收有效数据包数
	static int sendPacket;//发送的数据包数
	static int loginNum;//完成的登录业务数
	static int getVideoListNum;//获取用户视频列表的业务数
	static int uploadVideoPlayRecordNum;//上传用户的历史播放视频记录业务数
	static int uploadFileNum;//完成文件上传的业务数



private:
	DataManager() = delete;
	~DataManager() = delete;
};

