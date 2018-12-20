#include <assert.h>
#include <arpa/inet.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <memory.h>

#include "socketBus.h" 

void socketBus::startListening(const char* ip, int port) 
{
    Bind(ip, port);
    Listen(10);
}



//跨物理器发送数据 使用阻塞套接字
int socketBus::sendTo(const char* str, int connfd) 
{
    int res;
    do 
    {
        res = send(connfd, (void *)&str, sizeof(str), 0);
        if (res == -1) 
        {
            if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) 
            {
                continue;
            }
            else 
            {
                break;
            }
        }
        else 
        {
            break;
        }
    } while (1);
    return res;
}



//接收收到跨物理机发送的消息后中转udp通知本进程
void socketBus::recvFrom(void* buf, int length) 
{
    int fd = getMysockTCP(); 

    recv(fd, buf, length, 0);
}



int socketBus::makeNewConn(const char* destIP, int destPort) 
{
    int res = 0;
    int sockfd = 0;
    do 
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        assert(sockfd != -1);
        // setNonBlock(sockfd);
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        inet_pton(AF_INET, destIP, &addr.sin_addr);
        addr.sin_port = htons(destPort);
        int res = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
        if (res == -1) 
        {   
            perror("connect");
            close(sockfd);
        }

    } while (res == -1);    //防止connect失败
    return sockfd;
}

