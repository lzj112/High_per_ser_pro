#include<sys/types.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<poll.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include <fcntl.h>
#include<sys/epoll.h>
#include<vector>
#include<algorithm>
#include <string>
#include <iostream>
// #include"comm.h"


struct tmp 
{
    const int fd;
    int a;
    int b;
    std::string c;
    std::vector<int> d;
};

void setfdisblock(int fd, bool isblock)
{
    int flags = fcntl(fd, F_GETFL);
    if(flags < 0)
        return;
    if(isblock) // 阻塞
    {
        flags &= ~O_NONBLOCK;
    }
    else // 非阻塞
    {
        flags |= O_NONBLOCK;
    }    
    
    if(fcntl(fd, F_SETFL, flags)<0)
        perror("fcntl set");
}

typedef std::vector<struct epoll_event> EventList;
#define CLIENTCOUNT 2048
#define MAX_EVENTS 2048

int main(int argc, char **argv)
{
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd < 0)
    {
        perror("socket");
        return -1;
    }
    
    unsigned short sport = 8080;
    if(argc == 2)
    {
        sport = atoi(argv[1]);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    printf("port = %d\n", sport);
    addr.sin_port = htons(sport);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    if(bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        return -2;
    }
    if(listen(listenfd, 20) < 0)
    {
        perror("listen");
        return -3;
    }
        
    struct sockaddr_in connaddr;
    socklen_t len = sizeof(connaddr);
    
    int i = 0, ret = 0;
    std::vector<int> clients; // 客户端存储的迭代器
    int epollfd = epoll_create1(EPOLL_CLOEXEC);
    //int epollfd = epoll_create(MAX_EVENTS);// 设置连接数
    
    struct epoll_event event;
    event.events = EPOLLIN|EPOLLET;
    event.data.fd = listenfd;
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event) < 0)
    {
        perror("epoll_ctl");
        return -2;
    }
    EventList events(16);
    int count = 0;
    int nready = 0;
    char buf[1024] = {0};
    int conn  = 0;
    while(1)
    {
        
        nready = epoll_wait(epollfd, &*events.begin(), static_cast<int>(events.size()), -1);
        if(nready == -1)
        {
            perror("epoll_wait");
                        return -3;
        }
        if(nready == 0) // 肯定不会走到这里，因为上面没设置超时时间
        {
            continue;
        }
        if((size_t)nready == events.size()) // 对clients进行扩容
            events.resize(events.size() * 2);
        for(i = 0; i < nready; i++)
        {

            struct tmp* t = static_cast<tmp *> (events[i].data.ptr);
            std::cout << "const fd = " << t->fd << " a= " << t->a << " b = " << t->b << " c=" << t->c << std::endl;
            // for (auto x : t->d) 
            // {
            //     std::cout << "d = " << x << std::endl;
            // }
            // if(events[i].data.fd == listenfd)
            // {
            //     conn = accept(listenfd, (struct sockaddr*)&connaddr, &len);
            //     if(conn < 0)
            //     {
            //         perror("accept");
            //         return -4;
            //     }
            //     char strip[64] = {0};
            //     char *ip = inet_ntoa(connaddr.sin_addr);
            //     strcpy(strip, ip);
            //     printf("client connect, conn:%d,ip:%s, port:%d, count:%d\n", conn, strip,ntohs(connaddr.sin_port), ++count);
        
            //     clients.push_back(conn);
            //     // 设为非阻塞
            //     setfdisblock(conn, false);
            //     // add fd in events
            //     event.data.fd = conn;// 这样在epoll_wait返回时就可以直接用了
            //     event.events = EPOLLIN|EPOLLET;
            //     epoll_ctl(epollfd, EPOLL_CTL_ADD, conn, &event);
                
            // }
            // else if(events[i].events & EPOLLIN)
            // {
            //     conn = events[i].data.fd;
            //     if(conn < 0)
            //         continue;
            //     ret = read(conn, buf, sizeof(buf));
            //     if(ret == -1)
            //     {
            //         perror("read");
            //         return -5;
            //     }
            //     else if(ret == 0)
            //     {
            //         printf("client close remove:%d, count:%d\n", conn, --count);
            //         close(conn);
            //         event = events[i];
            //         epoll_ctl(epollfd, EPOLL_CTL_DEL, conn, &event);
            //         clients.erase(std::remove(clients.begin(), clients.end(), conn), clients.end());
            //     }
            //     write(conn, buf, sizeof(buf));
            //     memset(buf, 0, sizeof(buf));
            // }
        }    
    }
    close(listenfd);
    return 0;
}