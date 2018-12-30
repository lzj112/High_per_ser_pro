#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

#define IP "127.0.0.1"
// #define PORT 5000
#define PORT 5001    //正确的server端口


void isSelfConnection(int fd) 
{
    struct sockaddr_in tmp;
    socklen_t len = sizeof(tmp);

    for (int i = 0; i < 2; i++) 
    {
        memset(&tmp, 0, len);
        if (i == 1) 
            getpeername(fd, (struct sockaddr *)&tmp, &len);
        else 
            getsockname(fd, (struct sockaddr *)&tmp, &len);

        char ip[10];
        struct in_addr in = tmp.sin_addr;
        inet_ntop(AF_INET, &in, ip, sizeof(ip));
        if (i == 1)
            printf("ser ip-%s port-%d\n", ip, ntohs(tmp.sin_port));
        else 
            printf("cli ip-%s port-%d\n", ip, ntohs(tmp.sin_port));
    }
}

int main(int argc, char* argv[]) 
{
    //初始化地址
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &address.sin_addr);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);
    
    int optval = 1;
    int ret = setsockopt(sock,
                           SOL_SOCKET,
                           SO_REUSEADDR,
                           &optval,
                           sizeof(optval));
    if (ret == -1) 
    {
        perror("setReuseAddr falied ");
    }

    // /*  
    ret = bind(sock, 
               (struct sockaddr*)&address, 
               sizeof(address));
    // */
    ret = connect(sock, 
                  (struct sockaddr*)&address,
                  sizeof(address));
    if (ret == 0) 
    {
        printf("连接成功\n");
        isSelfConnection(sock);
    }
    else 
    {
        printf("未知错误\n");
    }

    errno = 0;
    ret = connect(sock, 
                  (struct sockaddr*)&address,
                  sizeof(address));
    if (errno == EISCONN) 
    {
        printf("端口已经被占用\n");
        isSelfConnection(sock);
    }
    else 
    {
        printf("errno = %d\n", errno);
    }

   return 0;
}