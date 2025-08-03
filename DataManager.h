#pragma once
#include <map>
#include <list>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
using namespace std;
class DataManager
{
public:

	static pthread_mutex_t mutex;
	static map<string, string> messageCodeMap;//手机号,验证码映射表
	//当前客户端连接<account,{fd,lastServerTime}>
	typedef struct ClientValue {
		string account;//账号
		string lastServerTime;//上次服务时间
	}ClientValue;
	static map<int,unique_ptr<ClientValue>> heartServiceMap;//心跳服务映射表
	static int rcvPacket;//接收有效数据包数
	static int sendPacket;//发送的数据包数
	static int loginNum;//完成的登录业务数
	static int registerNum;//完成的注册业务数
	static int getVideoListNum;//获取用户视频列表的业务数
	static int uploadVideoPlayRecordNum;//上传用户的历史播放视频记录业务数
	static int uploadVideoInfoNum;//上传视频信息业务数
	static int uploadFileNum;//完成文件上传的业务数



private:
	DataManager() = delete;
	~DataManager() = delete;
};

