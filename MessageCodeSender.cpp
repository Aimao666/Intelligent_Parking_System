#include "MessageCodeSender.h"
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <cstdlib>
#include <ctime>

// 单例模式实现
MessageCodeSender& MessageCodeSender::getInstance() {
    static MessageCodeSender instance;
    return instance;
}

MessageCodeSender::MessageCodeSender() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

MessageCodeSender::~MessageCodeSender() {
    if (m_sockfd != -1) {
        close(m_sockfd);
    }
}

void MessageCodeSender::init(const std::string& apiAccount,
    const std::string& apiKey,
    bool useHttps) {
    m_apiAccount = apiAccount;
    m_apiKey = apiKey;
    m_useHttps = useHttps;
}

int MessageCodeSender::sendVerificationCode(const std::string& mobile, string code) {
    if (code.size() < 4 || code.size() > 8) {
        return -1; // 无效验证码长度
    }

    std::string content = "您的验证码是：" + code +
        "。请不要把验证码泄露给其他人。";

    return sendCustomMessage(mobile, content);
}

int MessageCodeSender::sendCustomMessage(const std::string& mobile,
    const std::string& content) {
    // 检查短信长度 (GB/T 19000-2012规定单条短信≤70字符)
    if (content.length() > 350) { // UTF-8中文字符通常占3字节
        return -2;
    }

    // URL编码参数
    std::string accountEnc = CTools::urlEncode(m_apiAccount);
    std::string passwordEnc = CTools::urlEncode(m_apiKey);
    std::string mobileEnc = CTools::urlEncode(mobile);
    std::string contentEnc = CTools::urlEncode(content);

    // 构建POST数据
    std::string postData = "account=" + accountEnc +
        "&password=" + passwordEnc +
        "&mobile=" + mobileEnc +
        "&content=" + contentEnc;

    // 发送请求
    std::string response;
    int ret = httpPost("/webservice/sms.php?method=Submit&format=json",
        postData, response);

    // TODO: 解析JSON响应 {"code": 2, "msg": "提交成功",...}
    return ret;
}

// ======== 私有实现方法 ========
int MessageCodeSender::establishConnection() {
    if (m_sockfd != -1) {
        // 复用现有连接
        return m_sockfd;
    }

    addrinfo hints = { 0 }, * result;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // 解析DNS
    if (getaddrinfo(m_host.c_str(),
        m_useHttps ? "443" : "80",
        &hints, &result) != 0) {
        return -1;
    }

    // 尝试连接
    int sockfd = -1;
    for (addrinfo* rp = result; rp != nullptr; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd == -1) continue;

        // 设置超时
        timeval timeout = { CONN_TIMEOUT_SEC, 0 };
        setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break; // 连接成功
        }

        close(sockfd);
        sockfd = -1;
    }

    freeaddrinfo(result);

    if (sockfd != -1) {
        m_sockfd = sockfd; // 缓存连接
    }

    return sockfd;
}

int MessageCodeSender::httpPost(const std::string& endpoint,
    const std::string& postData,
    std::string& response) {
    // 建立连接
    int sockfd = establishConnection();
    if (sockfd < 0) {
        return -101; // 连接失败
    }

    try {
        // 构造HTTP请求
        std::string request = "POST " + endpoint + " HTTP/1.1\r\n"
            "Host: " + m_host + "\r\n"
            "Content-Type: application/x-www-form-urlencoded\r\n"
            "Content-Length: " + std::to_string(postData.size()) + "\r\n"
            "Connection: keep-alive\r\n\r\n" +
            postData;

        // 发送请求
        if (write(sockfd, request.c_str(), request.size()) <= 0) {
            close(sockfd);
            m_sockfd = -1;
            return -102; // 发送失败
        }

        // 读取响应
        char buffer[2048];
        ssize_t bytesRead;
        while ((bytesRead = read(sockfd, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0';
            response.append(buffer);
        }

        return 0; // 成功
    }
    catch (...) {
        close(sockfd);
        m_sockfd = -1;
        return -103; // 未知错误
    }
}

