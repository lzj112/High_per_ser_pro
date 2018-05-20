//统一事件源的简单实现

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

#define MAX_EVENT_NUMBER 1024
static int pipefd[2];

int setnonblocking(int fd) //设置非阻塞
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

void addfd(int epollfd, int fd) //注册新的fd到epfd
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

//信号处理函数
void sig_handler(int sig) 
{
    //保留原来的errno,在函数最后恢复,以保证函数的可重入性
    int save_errno = errno;
    int msg = sig;
    send(pipefd[1], (char*)&msg, 1, 0); //将信号值写入管道,已通知主循环
    errno = save_errno;
}

//设置信号的处理函数
void addsig(int sig) 
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = sig_handler;
    sa.sa_flags |= SA_RESTART;
    sigfillset(&sa.sa_mask); //在信号集中设置所有信号
    assert(sigaction(sig, &sa, NULL) != -1);

    //sigfillset()用来将参数set信号集初始化，
    //然后把所有的信号加入到此信号集里即将所有的信号标志位置为1，屏蔽所有的信号。


    //int sigaction(int sig, const struct sigaction* act, struct sigaction* oact);
    //结构体sigaction中的成员sa_handler制定信号处理函数,sa_mask设置进程信号掩码(在进程原有信
    //号掩码的基础上增加信号掩码),以指定哪些信号不能发送给本进程,sa_flags设置程序收到信号时的行为
    //上述的SA_RESTART已经过时,最好不要使用
}

int main(int argc, char* argv[]) 
{
    if (argc <= 2) 
    {
        printf("参数不够\n");
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi(argv[2]);

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ip, &address.sin_addr);

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);

    ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(listenfd, 5);
    assert(ret != -1);

    //epoll
    struct epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(5);
    assert(epollfd != -1);
    addfd(epollfd, listenfd);

    //使用socketpair创建管道,注册pipefd[0]上的可读事件
    //socketpair是一对相互连接的socket，相当于服务器端和客户端的两个端点，每一端都可以读写数据。
    ret = socketpair(PF_UNIX, SOCK_STREAM, 0, pipefd);
    assert(ret != -1);
    setnonblocking(pipefd[1]); //pipefd[1]拿来写,设置为非阻塞
    addfd(epollfd, pipefd[0]); //将pipefd[0]添加到监听集合

    //设置一些信号处理函数
    addsig(SIGHUP);
    addsig(SIGCHLD);
    addsig(SIGTERM);
    addsig(SIGINT);
    bool stop_server = false;

    while (!stop_server) 
    {
        int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if ((number < 0) && (errno != EINTR)) 
        {
            printf("epoll failure\n");
            break;
        }

        for (int i = 0; i < number; i++) 
        {
            int sockfd = events[i].data.fd;

            //如果就绪的文件描述符是listenfd,则处理新的连接
            if (sockfd == listenfd) 
            {
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof(client_address);
                int connfd = accept(listenfd, (struct sockaddr*)&client_address, 
                                    &client_addrlength);
                addfd(epollfd, connfd);
            }

            //如果就绪的文件描述符是pipefd[0],处理信号
            else if ((sockfd == pipefd[0]) && (events[i].events & EPOLLIN)) 
            {
                int sig;
                char signals[1024];
                ret = recv(pipefd[0], signals, sizeof(signals), 0); //发过来的是信号值
                if (ret == -1) 
                {
                    continue;
                }
                else if (ret == 0) 
                {
                    continue;
                }
                else 
                {
                    //因为每个信号占一字节,所以按字节来逐个接受信号,我们以SIGTERM
                    //为例,来说明如何安全地终止服务器主循环
                    for (int i = 0; i < ret; i++) 
                    {
                        switch(signals[i])
                        {
                            case SIGCHLD: //子进程状态发生变化(暂停或退出)
                            case SIGHUP: //控制终端挂起
                            {
                                continue;
                            }
                            case SIGTERM: //终止进程
                            case SIGINT: //键盘输入以中断进程(Ctrl+C)
                            {
                                stop_server = true;
                            }
                        }
                    }
                }
            }
            else 
            {

            }
        }
    }
    printf("close fds\n");
    close(listenfd);
    close(pipefd[0]);
    close(pipefd[1]);
    return 0;
}

