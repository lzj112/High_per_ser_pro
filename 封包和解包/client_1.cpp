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

int MySend(int iSock, char* pchBuf, size_t tLen) 
{
    int iThisSend;
    unsigned int iSended = 0;
    if (tLen == 0) 
    {
        return 0;
    }
    while (iSended < tLen) 
    {
        do 
        {
            iThisSend = send(iSock, pchBuf, tLen - iSended, 0);
        } while ((iThisSend < 0) && (errno == EINTR));
        if (iThisSend < 0) 
        {
            return iSended;
        }
        iSended += iThisSend;
        pchBuf += iThisSend;
    }
    return tLen;
}

int main(int argc, char* argv[]) 
{
    int ret = 0;
    int connfd = 0;
    int cLen = 0;
    struct sockaddr_in client;
    if (argc < 2) 
    {
        return -1;
    }
    client.sin_family = AF_INET;
    client.sin_port = htons(DEFAULT_PORT);
    inet_pton(AF_INET, argv[1], &client.sin_addr);
    // client.sin_addr.s_addr = inet_addr(argv[1]);
    connfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(connfd >= 0);

    ret = connect(connfd, (struct sockaddr*)&client, sizeof(client));
    assert(ret >= 0); 
    
    ssize_t writeLen;
    const char* sendMsg = "0123456789";
    int tLen = strlen(sendMsg);
    printf("tLen : %d\n", tLen);
    int iLen = 0;
    char* pBuff = new char[100];
    *(int*)(pBuff + iLen) = htonl(tLen);    //转换为网络字节序存入前四个字节
    iLen += sizeof(int);    //将字符串前4个字节表示字符串大小
    memcpy(pBuff + iLen, sendMsg, tLen);  //将数据粘到后面
    iLen += tLen;
    writeLen = MySend(connfd, pBuff, iLen);
    if (writeLen < 0) 
    {
        printf("write failed\n");
        close(connfd);
        return 0;
    }
    else 
    {
        printf("write success, writelen : %d, sendMsg : %s\n", writeLen, sendMsg);
    }
    close(connfd);
    return 0;
}