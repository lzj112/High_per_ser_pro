//加包头解决粘包问题
//发送一个字符串
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>

#define DEFAULT_PORT 6666

int MyRecv(int iSock, char* pchBuf, size_t tCount) 
{
    size_t tBytesRead = 0;
    int iThisRead;
    while (tBytesRead < tCount) 
    {
        do 
        {
            iThisRead = read(iSock, pchBuf, tCount - tBytesRead);
        } while ((iThisRead < 0) && (errno == EINTR));
        if (iThisRead < 0) 
        {
            return iThisRead;
        }
        else if (iThisRead == 0) 
        {
            return tBytesRead;
        }
        tBytesRead += iThisRead;
        pchBuf += iThisRead;
    }
}

int main(int argc, char* argv[]) 
{
    int sockfd, acceptfd;
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    unsigned int sin_size, myport = 6666, lisnum = 10;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("socket");
        return -1;    
    }
    printf("sockfd is ok\n");
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(DEFAULT_PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero), 0);

    if (bind(sockfd, (struct sockaddr*)&my_addr, sizeof(my_addr)) == -1)
    {
        perror("bind");
        return -2;
    }
    printf("bind is ok\n");
    if (listen(sockfd, lisnum) == -1) 
    {
        perror("listen");
        return -3;
    }
    printf("listen is ok\n");

    char recvMsg[10];
    sin_size = sizeof(my_addr);
    acceptfd = accept(sockfd, (struct sockaddr*)&my_addr, &sin_size);
    if (acceptfd < 0) 
    {
        close(sockfd);
        printf("accept failed\n");
        return -1;
    }
    ssize_t readLen = MyRecv(acceptfd, recvMsg, sizeof(int)); //第一次接收表示数据长度的四个字节
    if (readLen < 0) 
    {
        printf("read failed\n");
        return -1;
    }
    int len = (int)ntohl(*(int*)recvMsg); //转换为主机字节序
    printf("len : %d\n", len);
    readLen = MyRecv(acceptfd, recvMsg, len); //再次接收全部数据
    if (readLen < 0) 
    {
        printf("read failefd\n");
        return -1;
    }
    recvMsg[len] = '\0';
    printf("recvMsg : %s\n", recvMsg);
    close(sockfd);
    return 0;
}