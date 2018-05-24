#define TIMEOUT 5000

int timeout = TIMEOUT;
time_t start = time(NULL);
time_t end = time(NULL);

while (1) 
{
    printf("the timeout is now %d mil-seconds\n", timeout);
    start = time(NULL);
    int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, timeout);
    if ((number < 0) && (errno != EINTR)) 
    {
        printf("epoll failure\n");
        break;
    }
    //如果epoll_wait成功返回0,则说明超时时间到，此时便可处理定时任务，并重置定时时间
    if (number == 0) //超时时间内无任务
    {
        timeout = TIMEOUT; //重置超时时间
        continue;
    }

    end = time(NULL);

    //如果epoll_wait的返回值大于0，则本次epoll_wait调用持续的时间（end-start）*1000ms
    //我们需要将定时时间timeout减去这段时间，以获得下次epoll_wait调用的超时时间
    timeout -= (end - start) * 1000;

    //超时时间没用完就继续用 <=0的时候说明用完了，要重置
    //重新计算之后的timeout值有可能等于0,说明本次epoll_wait调用返回时，不仅有文件描述符就绪
    //而且其超时时间也刚好到达，此时我们也要处理定时任务，并重置定时时间
    if (timeout <= 0) 
    {
        timeout = TIMEOUT;
    }

    //handle connections
}