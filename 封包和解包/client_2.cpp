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
    int connfd = 0;
    int cLen = 0;
    int ret = 0;

    struct sockaddr_in client;
    if (argc < 2) 
    {
        printf("error\n");
        return -1;
    }
    
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr(argv[1]);
    client.sin_port = htons(DEFAULT_PORT);
    
    connfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(connfd >= 0);

    ret = connect(connfd, (struct sockaddr*)&client, sizeof(client));
    assert(ret >= 0);

    Pkg mypkg;
    mypkg.head.num = 1;
    mypkg.head.index = 10001;
    mypkg.content.sex = 'm';
    mypkg.content.score = 90;
    char* temp = "guangzhou and shanghai";
    strncpy(mypkg.content.address, temp, sizeof(mypkg.content.address));
    mypkg.content.age = 18;
    
    ssize_t writeLen;
    int tLen = sizeof(mypkg);
    printf("tLen : %d\n", tLen);
    int iLen = 0;
    char* pBuff = new char[1000];
    *(int *)(pBuff + iLen) = htonl(tLen);
    iLen += sizeof(int);
    
    *(int *)(pBuff + iLen) = htonl(mypkg.head.num);
    iLen += sizeof(int);
    *(int *)(pBuff + iLen) = htonl(mypkg.head.index);
    iLen += sizeof(int);
    memcpy(pBuff+iLen, &mypkg.content.sex, sizeof(char));
    iLen += sizeof(char);
    *(int *)(pBuff + iLen) = htonl(mypkg.content.score);
    iLen += sizeof(int);
    memcpy(pBuff+iLen, mypkg.content.address, sizeof(mypkg.content.address));
    iLen += sizeof(mypkg.content.address);
    *(int *)(pBuff + iLen) = htonl(mypkg.content.age);
    iLen += sizeof(int);
    
    writeLen = MySend(connfd, pBuff, iLen);
    assert(writeLen >= 0);

    printf("write success, writelen : %d, pBuff : %s\n", writeLen, iLen, pBuff);
    
    close(connfd);
    return 0;
}