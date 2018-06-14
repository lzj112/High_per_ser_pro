//加包头解决粘包问题
//发送一个结构体
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
#include "define.h"

#define DEFAULT_PORT 6666

int main(int argc, char* argv[]) 
{
    int sockfd, acceptfd; 
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    unsigned int sin_size, myport = 6666, lisnum = 10;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
        perror("socket");
        return -1;
    }
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(DEFAULT_PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero), 0);

    int ret = 0;
    ret = bind(sockfd, (struct sockaddr*)&my_addr, sizeof(my_addr));
    assert(ret != -1);

    ret = listen(sockfd, lisnum);
    assert(ret != -1);

    char recvMsg[1000];
    sin_size = sizeof(my_addr);
    ret = accept(sockfd, (struct sockaddr*)&my_addr, &sin_size);
    assert(ret >= 0);

    ssize_t readLen = MyRecv(acceptfd, recvMsg, sizeof(int));
    assert(readLen >= 0);

    int len = (int)ntohl(*(int*)recvMsg);
    printf("len : %d\n", len);
    readLen = MyRecv(acceptfd, recvMsg, len);
    assert(readLen >= 0);

    char* pBuff = recvMsg;
    Pkg RecvPkg;
    int iLen = 0;
    
    memcpy(&RecvPkg.head.num, pBuff+iLen, sizeof(int));
    iLen += sizeof(int);
    RecvPkg.head.num = ntohl(RecvPkg.head.num);
    printf("REcvPkg.head.num: %d\n", RecvPkg.head.num);
    
    memcpy(&RecvPkg.head.index, pBuff+iLen, sizeof(int));
    iLen += sizeof(int);
    RecvPkg.head.index = ntohl(RecvPkg.head.index);
    printf("RecvPkg.head.index : %d\n", RecvPkg.head.index);

    memcpy(&RecvPkg.content.sex, pBuff+iLen, sizeof(char));
    iLen += sizeof(char);
    printf("RecvPkg.content.sex : %c\n", RecvPkg.content.sex);

    memcpy(&RecvPkg.content.score, pBuff+iLen, sizeof(int));
    iLen += sizeof(int);
    RecvPkg.content.score = ntohl(RecvPkg.content.score);
    printf("RecvPkg.content.score : %d\n", RecvPkg.content.score);

    memcpy(&RecvPkg.content.address, pBuff+iLen, sizeof(RecvPkg.content.address));
    iLen += sizeof(RecvPkg.content.address);
    printf("RecvPkg.content.address : %s\n", RecvPkg.content.address);

    memcpy(&RecvPkg.content.age, pBuff+iLen, sizeof(int));
    iLen += sizeof(int);
    RecvPkg.content.age = ntohl(RecvPkg.content.age);
    printf("RecvPkg.content.age : %d\n", RecvPkg.content.age);

    close(acceptfd);
    return 0;
}