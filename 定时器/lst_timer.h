//升序定时器链表
#ifndef LST_TIMER_H
#define LST_TIMER_H

#include <netinet/in.h>
#include <stdio.h>
#include <time.h>
#define BUFFER_SIZE 64
class util_timer; //前向声明

struct client_data 
{
    struct sockaddr_in address;
    int sockfd;
    char buf[BUFFER_SIZE];
    util_timer* timer;
};

//定时器类
class util_timer 
{
public:
    util_timer() : prev(nullptr), next(nullptr) {}
    time_t expire; //任务的超时时间，这里使用绝对时间
    void (*cb_func) (client_data*); //函数指针 任务回调函数
    //回调函数处理的客户数据，由定时器的执行者传递给回调函数
    client_data* user_data;
    util_timer* prev; //指向前一个定时器
    util_timer* next; //指向下一个定时器
};

//定时器链表。它是一个升序、双向链表，且带有头结点和尾节点
class sort_timer_lst 
{
    util_timer* head;
    util_timer* tail;
    
    //一个重载的辅助函数，他被公有的add_timer函数和adjust_timer函数调用。
    //该函数表示将目标定时器timer添加到节点lst_head之后的部分链表
    void add_timer(util_timer* timer, util_timer* lst_head);

public:
    sort_timer_lst() : head(nullptr), tail(nullptr) {}
    //链表被销毁时，删除其中所有的定时器
    ~sort_timer_lst();

    //将目标定时器timer添加到链表中
    void add_timer(util_timer* timer);
 
    //当某个定时任务发生变化时，调整对应的定时器在链表中的位置。这个函数
    //只考虑被调整的定时器的超时时间延长的情况，即该定时器需要往链表的尾部移动
    void adjust_timer(util_timer* timer);

    //将目标定时器从链表中删除
    void del_timer(util_timer* timer);

    //SIGALRM信号每次被触发就在其信号处理函数（如果使用统一事件源，则是主函数）
    //中执行一次tick函数，已处理链表上到期的任务
    void tick();
};

sort_timer_lst::~sort_timer_lst() 
{
    util_timer* tmp = head;
    while (tmp) 
    {
        head = tmp->next;
        delete tmp;
        tmp = head;
    }
}

void sort_timer_lst::add_timer(util_timer* timer) 
{
    if (!timer) 
    {
        return;
    }
    if (!head) //若果链表空
    {
        head = tail = timer;
        return;
    }

    //如果目标定时器的超时时间小于当前链表中所有定时器的超时时间
    //则把该定时器插入链表头部，作为新的链表头结点。否则就需要重载函数
    //add_timer(util_timer* timer, util_timer* lst_head)，把他
    //插入链表中合适的位置，以保证链表的升序特性
    if (timer->expire < head->expire) 
    {
        timer->next = head;
        head->prev = timer;
        head = timer;
        return ;
    }
    add_timer(timer, head);
}

void sort_timer_lst::adjust_timer(util_timer* timer) 
{
    if (!timer) 
    {
        return;
    }
    util_timer* tmp = timer->next;
    //如果被调整的目标定时器在链表尾部，或者该定时器新的超时值仍然小于其下一个
    //定时器的超时值，则不用调整
    if (!tmp || (timer->expire < tmp->expire)) 
    {
        return ;
    }
    //如果目标定时器是链表的头节点，则将该定时器从链表中取出并重新插入链表
    if (timer == head) 
    {
        head = head->next;
        head->prev = nullptr;
        timer->next = nullptr;
        add_timer(timer, head);
    }
    //如果目标定时器不是链表头节点，则将该定时器从链表中取出，然后插入其原来所在位置之后的部分链表中
    else 
    {
        timer->prev->next = timer->next;
        timer->next->prev = timer->prev;
        add_timer(timer, timer->next);
    }
}

void sort_timer_lst::del_timer(util_timer* timer) 
{
    if (!timer) 
    {
        return ;
    }
    //下面这个条件成立表示链表中只有一个定时器，即目标定时器
    if ((timer == head) && (timer == tail)) 
    {
        delete timer;
        head = nullptr;
        tail = nullptr;
        return ;
    }
    //如果链表中至少有两个定时器，且目标定时器是链表的头节点，则将链表的头节点
    //重置为原头节点的下一个节点
    if (timer == head) 
    {
        head = head->next;
        head->prev = nullptr;
        delete timer;
        return ;
    }
    //如果是尾节点
    if (timer == tail) 
    {
        tail->prev->next = nullptr;
        tail = tail->prev;
        delete timer;
        return ;
    }
    //如果位于链表中间，则将其前后定时器串联起来
    timer->prev->next = timer->next;
    timer->next->prev = timer->prev;
    delete timer;
}

void sort_timer_lst::tick() 
{
    if (!head) 
    {
        return ;
    }
    printf("timer tick : \n");
    time_t cur = time(nullptr); //获得系统当前时间
    util_timer* tmp = head;
    
    //从头节点开始依次处理每个定时器，直到遇到一个尚未到期的定时器，
    //这就是定时器的核心逻辑
    while (tmp) 
    {
        //因为每个定时器都使用绝对时间作为超时值，所以我们可以把定时器的超时值和
        //系统当前时间，比较以判断定时器是否到期
        if (cur < tmp->expire) 
        {
            break;
        }
        //调用定时器的回调函数，以执行定时任务
        tmp->cb_func(tmp->user_data);

        //执行完定时器中的定时任务之后，就将它从链表中删除，并重置链表头节点
        head = tmp->next;
        if (head) 
        {
            head->prev = nullptr;
        }
        delete tmp;
        tmp = head;
    }
}

void sort_timer_lst::add_timer(util_timer* timer, util_timer* lst_head) 
{
    util_timer* prev = lst_head;
    util_timer* tmp = prev->next;
    //遍历lst_head节点之后的部分链表，直到找到一个超时时间大于目标定时器的超时时间的节点，
    //病将目标定时器插入该节点之前
    while (tmp) 
    {
        if (timer->expire < tmp->expire) 
        {
            prev->next = timer;
            timer->next = tmp;
            timer->prev = prev;
            tmp->prev = timer;
        }
        prev = tmp;
        tmp = tmp->next;
    }
    //遍历结束仍未找到超时时间大于目标定时器超时时间的节点，插入尾部
    if (!tmp) 
    {
        prev->next = timer;
        timer->prev = prev;
        timer->next = nullptr;
        tail = timer;
    }
}
#endif