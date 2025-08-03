#include "DataManager.h"
pthread_mutex_t DataManager::mutex;
map<string, string> DataManager::messageCodeMap;//手机号,验证码映射表
map<int, unique_ptr<DataManager::ClientValue>> DataManager::heartServiceMap;//心跳服务映射表
int DataManager::rcvPacket;//接收有效数据包数
int DataManager::sendPacket;//发送的数据包数
int DataManager::loginNum;//完成的登录业务数
int DataManager::registerNum;//完成的注册业务数
int DataManager::getVideoListNum;//获取用户视频列表的业务数
int DataManager::uploadVideoPlayRecordNum;//上传用户的历史播放视频记录业务数
int DataManager::uploadVideoInfoNum;//上传视频信息业务数
int DataManager::uploadFileNum;//完成文件上传的业务数