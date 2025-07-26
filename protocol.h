#pragma once
//请求头+请求体
//请求头+返回体
// 时间格式yyyy - mm - dd hh : MM:ss
//业务类型,单数表示客服端->服务器，双数是服务器->客户端
#include <iostream>
using namespace std;
const size_t MAX_BODY_LENGTH = 5 * 1024; // 5kB
const int MAX_BUSINESS_TYPE = 100; // 根据协议调整
typedef struct HEAD {
	int bussinessType;//请求体业务类型
	int bussinessLength;//请求体字节长度
	uint32_t crc;//请求体CRC校验码
}HEAD;

//通用返回体
typedef struct CommonBack {
	int flag;//登录结果
	char message[50];//信息
}CommonBack;


//登录请求体1
typedef struct LoginRequest {
	char account[12];//登录用手机号
	char password[80];//登录用密码
}LoginRequest;


//获取验证码请求体3
typedef struct CodeRequest {
	char account[12];//手机号
}CodeRequest;
//验证码返回体4


//注册请求体5
typedef struct RegisterRequest {
	char account[12];//登录用手机号
	char code[8];//短信验证码
}RegisterRequest;
//注册返回体 6


//入场请求体7
typedef struct CarEntryRequest {
	char account[12];//用户账号,服务器要用这个字段来区分不同用户的数据
	char entryTime[24];//入场时间
	char carNumber[10];//车牌号
	char entryPosition[20];//入场位置
}CarEntryRequest;
//入场返回体8


//出场 - 识别车牌请求体9 - 客户端要获取该车的停车时长和应收金额
typedef struct CarLeaveRequest {
	char account[12];//用户账号,服务器要用这个字段来区分不同用户的数据
	char leaveTime[24];//出场时间
	char carNumber[10];//车牌号
	char leavePosition[20];//出场位置
}CarLeaveRequest;
//出场 - 识别车牌返回体10 - 客户端要获取该车的停车时长和应收金额 
typedef struct CarLeaveBack {
	char entryTime[24];//出场时间
	long long mesc;//停车时长（秒数）
	char carNumber[10];//车牌号
	int dueCost;//应收金额
}CarLeaveBack;


//放行请求体 11
typedef struct AgreeLeaveRequest {
	char account[12];//用户账号,服务器要用这个字段来区分不同用户的数据
	int reallyCost;//实际缴费金额
	char carNumber[10];//车牌号
}AgreeLeaveRequest;
//放行返回体 12


//上传录制视频信息请求体13
typedef struct UploadVideoRequest {
	char account[12];//用户账号,服务器要用这个字段来区分不同用户的数据
	char name[40];//视频名称
	char path[80];//客户端视频路径
	char totaltime[10];//视频时长3:23:59
	char createtime[24];//创建时间
}UploadVideoRequest;
//上传录制视频信息返回体14


//获取服务器所存储的视频日期 请求体 通过客户端传标志表示按月查还是按天查 15
typedef struct VideoDateRequest {
	char account[12];//用户账号,服务器要用这个字段来区分不同用户的数据
	int queryFlag;//查询标志，传0表示按天查，传1表示按月查
	int year;//年份 如传2024表示查2024年有哪些天或哪些月有数据
}VideoDateRequest;
//获取服务器所存储的视频日期 返回体 服务端返回在数据库中哪些天或者哪些月份有视频数据16
typedef struct VideoDateBack {
	int queryFlag;      // 查询标志：0-按天 1-按月
	int year;           // 查询年份
	int count;          // 有效数据数量
	char dates[366][11]; // 日期数组：YYYY-MM-DD或YYYY-MM格式
} VideoDateBack;

//==== = 分页查询==== =
//获取视频播放列表请求体  17
typedef struct VideoListRequest {
	char account[12];//用户账号,服务器要用这个字段来区分不同用户的数据
	int requestPage;//请求要第几页数据，页数从1开始数
	int queryFlag;// 查询标志：0-按天 1-按月
	char dateTime[11];//查询时间按天YYYY-MM-DD或按月YYYY-MM格式
}GETREPLAYINFO;
//单个视频信息结构体
typedef struct VideoData {
	int videoId;//视频id
	char createtime[24];//视频创建时间 
	char name[40];//视频名称
	char videoPath[80];//客户端视频路径
	char totaltime[10];//视频总时长 格式为3:23:59
	char currentPlaytime[10];//当前播放时长 格式为3:23:59
	char picturePath[80];//客户端视频首帧图片路径
}VideoData;
//获取视频播放列表返回体 18
typedef struct VideoListBack {
	int num; //有效数据个数
	int currentPage;//当前页
	VideoData videoDataArr[6];//视频信息列表
}VideoListBack;


//上传播放视频信息请求体19
typedef struct UploadPlayinfoRequest {
	char account[12];//用户账号,服务器要用这个字段来区分不同用户的数据
	int videoId;//视频id
	char currenttime[10];//当前播放时长3:23:59
}UploadPlayinfoRequest;
//上传播放视频信息返回体20

/*========== = 这部分取消了============
获取历史播放进度 21
typedef struct getreplayinfo {
	char account[12];//用户账号,服务器要用这个字段来区分不同用户的数据
	char name[40];//视频名称 这里用视频名称还是视频id
}GETREPLAYINFO;

typedef struct replayinfoback {
	char name[40];//视频名称  这里用视频名称还是视频id
	int mesc;//上次播放进度换算的秒数
}REPLAYINFOBACK;
=================================
*/
//车辆信息查询请求体 21
//车牌号，入场时间，出场时间至少填写一个，最多填写三个
//车牌号支持模糊查询，入场时间、出场时间表示 入场时间 >= 查询入场时间，出场时间 <= 查询出场时间
typedef struct ParkingInfoRequest {
	char account[12];//用户账号,服务器要用这个字段来区分不同用户的数据
	char carNumber[10];//车牌号
	char entryTime[24];//入场时间
	char leaveTime[24];//出场时间
	int currentPage;//当前页
}ParkingInfoRequest;
//单个停车车辆信息数据
typedef struct ParkingInfoData {
	int id;//停车记录id
	char carNumber[10];//车牌号
	char entryTime[24];//入场时间
	char leaveTime[24];//出场时间
	int dueCost;//应付金额
	int reallyCost;//实付金额
	char entryPicName[40];//入场图片名称
	char entryPicPath[80];//入场图片路径
	char leavePicName[40];//出场图片名称
	char leavePicPath[80];//出场图片路径
}ParkingInfoData;
//车辆信息查询返回体 22，用什么数据结构返回？一个包直接包含所有数据吗
typedef struct ParkingInfoBack {
	int num; //有效数据个数
	int currentPage;//当前页
	ParkingInfoData parkingInfoDataArr[6];//停车信息数组
}ParkingInfoBack;


//文件上传请求体 23
typedef struct FileInfoRequest {
	char account[12];//用户账号,服务器要用这个字段来区分不同用户的数据
	char filename[40];//图片名称
	int totalNumber;//原文件碎片总个数
	int totalLength;//原文件总字节数
	int fileIndex;//当前碎片的序号，从1开始
	int fileLength;//当前碎片的有效字节数
	char context[1000];//文件二进制内容
}FileInfoRequest;
//文件碎片发送完成以后，客户端还要发送一个确认发送完成请求包 25
typedef struct FileCheckRequest
{
	char account[12]; //文件归属账号
	char filename[40]; //文件名称
	int flag;
}FileCheckRequest;
//服务器如果收到这个文件确认包，验证自己收到所有该文件请求缓存数据是否符合文件碎片数量，序号是否都有，如果有缺失就将
//碎片文件包没有收全  服 -> 客
//服务器返回24
typedef struct FileBack
{
	char filename[40]; //文件名称
	int arr[100];//丢包序号，数组元素初始化为-1，客户端遍历数组发现元素值为-1表示后面的元素无效
}FileBack;
//服务器返回26
typedef struct FileSuccessBack
{
	char filename[40]; //文件名称
	int flag; //结果标志
}FileSuccessBack;
