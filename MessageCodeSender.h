#pragma once
#include <string>
#include <memory>
#include "CTools.h"
//接口类型：互亿无线触发短信接口，支持发送验证码短信、订单通知短信等。
// 账户注册：请通过该地址开通账户http://sms.ihuyi.com/register.html
// 注意事项：
//（1）调试期间，请用默认的模板进行测试，默认模板详见接口文档；
//（2）请使用APIID（查看APIID请登录用户中心->验证码短信->产品总览->APIID）及 APIkey来调用接口；
//（3）该代码仅供接入互亿无线短信接口参考使用，客户可根据实际需要自行编写；
class MessageCodeSender {
public:
    // 单例模式访问点
    static MessageCodeSender& getInstance();

    // 初始化配置（账号密码应外部注入）
    void init(const std::string& apiAccount,
        const std::string& apiKey,
        bool useHttps = false);

    /**
    * 发送短信验证码
    * @param mobile 目标手机号
    * @param code 验证码
    * @return 0:成功 非0:错误码
    */
    int sendVerificationCode(const std::string& mobile, string code);

    /**
    * 通用短信发送接口
    * @param mobile 目标手机号
    * @param content 短信内容
    * @return 0:成功 非0:错误码
    */
    int sendCustomMessage(const std::string& mobile, const std::string& content);

    // 禁用复制和赋值
    MessageCodeSender(const MessageCodeSender&) = delete;
    void operator=(const MessageCodeSender&) = delete;

private:
    MessageCodeSender();
    ~MessageCodeSender();

    // 内部实现方法
    int establishConnection();
    int httpPost(const std::string& endpoint,
        const std::string& postData,
        std::string& response);

    // 配置参数
    std::string m_apiAccount;
    std::string m_apiKey;
    std::string m_host = "106.ihuyi.cn";
    bool m_useHttps = false;

    // 连接状态
    int m_sockfd = -1;
    static const int CONN_TIMEOUT_SEC = 10;
};