//用SIGURG检测带外数据是否到达

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>

#define BUF_SIZE 1024

static int connfd;

//SIGURG信号处理函数
void sig_urg(int sig) 
{
    int save_errno = errno;
    char buffer[BUF_SIZE];
    memset(buffer, '\0', BUF_SIZE);
    int ret = recv(connfd, buffer, BUF_SIZE-1, MSG_OOB); //接受带外数据
    printf("got %d bytes of OOB DATA '%s'\n", ret, buffer);
    errno = save_errno;
}

void addsig(int sig, void(*sig_handler) (int)) 
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = sig_handler; // 指定处理函数
    sa.sa_flags |= SA_RESTART;
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
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

    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ip, &address.sin_addr);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock, 5);
    assert(ret != -1);

    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client_addrlength);
    connfd = accept(sock, (struct sockaddr*)&client_addrlength, &client_addrlength);
    if (connfd < 0) 
    {
        printf("errno is %d\n", errno);
    }
    else 
    {
        addsig(SIGURG, sig_urg); //接收带外数据
        //使用SIGURG信号之前，我们必须设置socket的宿主进程或进程组
        fcntl(connfd, F_SETOWN, getpid());

        char buffer[BUF_SIZE];
        while (1) //循环接收普通数据 
        {
            memset(buffer, '\0', BUF_SIZE);
            ret = recv(connfd, buffer, BUF_SIZE-1, 0);
            if (ret <= 0) 
            {
                break;
            }
            printf("got %d bytes of normal data '%s'\n", ret, buffer);
        }
        close(connfd);
    }
    close(sock);
    return 0;
}