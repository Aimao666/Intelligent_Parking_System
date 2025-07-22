#include "MessageCodeSender.h"
int MessageCodeSender::basefd;
char* MessageCodeSender::hostname = "106.ihuyi.cn";
char* send_sms_uri = "/webservice/sms.php?method=Submit&format=json";
//用户名是登录用户中心->验证码短信->产品总览->APIID
char* MessageCodeSender::account = "C65451499";

//查看密码请登录用户中心->验证码短信->产品总览->APIKEY
char* MessageCodeSender::password = "6bcebac852eaf1c5be1d3318e6c4674b";
MessageCodeSender::MessageCodeSender()
{
}

MessageCodeSender::~MessageCodeSender()
{
}

ssize_t MessageCodeSender::send_sms(char* mobile)
{
    string content = "智慧停车服务平台提醒您，您的验证码为" + CTools::itos(rand() % 1000000, 6) 
        + "，有效期为5分钟，请不要把验证码告诉其他人。";
    char params[MAXPARAM + 1];
    char* cp = params;
    sprintf(cp, "account=%s&password=%s&mobile=%s&content=%s", account, password, mobile, content);
    return http_post(send_sms_uri, cp);
}

ssize_t MessageCodeSender::http_post(char* page, char* poststr)
{
    char sendline[MAXLINE + 1], recvline[MAXLINE + 1];
    ssize_t n;
    snprintf(sendline, MAXSUB,
        "POST %s HTTP/1.0\r\n"
        "Host: %s\r\n"
        "Content-type: application/x-www-form-urlencoded\r\n"
        "Content-length: %zu\r\n\r\n"
        "%s", page, hostname, strlen(poststr), poststr);

    write(basefd, sendline, strlen(sendline));
    while ((n = read(basefd, recvline, MAXLINE)) > 0) {
        recvline[n] = '\0';
        printf("%s", recvline);
    }
    return n;
}

int MessageCodeSender::socked_connect(char* arg)
{
    struct sockaddr_in their_addr = { 0 };
    char buf[1024] = { 0 };
    char rbuf[1024] = { 0 };
    char pass[128] = { 0 };
    struct hostent* host = NULL;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("create the sockfd is failed\n");
        return -1;
    }

    if ((host = gethostbyname(arg)) == NULL)
    {
        printf("Gethostname error, %s\n");
        return -1;
    }

    memset(&their_addr, 0, sizeof(their_addr));
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(80);
    their_addr.sin_addr = *((struct in_addr*)host->h_addr);
    if (connect(sockfd, (struct sockaddr*)&their_addr, sizeof(struct sockaddr)) < 0)
    {
        close(sockfd);
        return  -1;
    }
    printf("connect is success\n");
    return sockfd;
}
