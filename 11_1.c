//设置connect超时时间
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>

//超时连接函数
int timeout_connect(const char* ip, int port, int time) 
{
    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ip, &address.sin_addr);

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);

    //通过选项SO_RCVTIMEO和SO_SNDTIMEO所设置的超时时间的类型是timeval,
    //这和select系统调用的超时时间参数类型相同
    struct timeval timeout;             //struct timeval
    timeout.tv_sec = time;              //{
    timeout.tv_usec = 0;                //  __time_t tv_sec;  long 类型 秒
    socklen_t len = sizeof(address);    //  __time_t tv_usec; long 类型 微秒 
                                        //}
    ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, len);
    assert(ret != -1);

    ret = connect(sockfd, (struct sockaddr*)&address, sizeof(address));
    if (ret == -1) 
    {
        //超时对应的错误号是EINPROGRESS,下面这个条件如果成立,我们就可以处理定时任务了
        if (errno == EINPROGRESS) 
        {
            printf("conneection timeout,process timeout logic\n");
            return -1;
        }
        printf("error occur when connection to server\n");
        return -1;
    }
    return sockfd;
}

int main(int argc, char* argv[]) 
{
    if (argc <= 2) 
    {
        printf("errno\n");
        return 1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);

    int sockfd = timeout_connect(ip, port, 10);
    if (sockfd < 0) 
    {
        return 1;
    }
    return 0;
}