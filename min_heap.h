//最小堆实现的定时器--时间堆
#ifndef _MIN_HEAP_H
#define _MIN_HEAP_H

#include <iostream>
#include <netinet/in.h>
#include <time.h>
using std::exception;

#define BUFFER_SIZE 64
class heap_timer; //前向声明

//用户数据
struct client_data 
{
    sockaddr_in address;
    int sockfd;
    char buf[BUFFER_SIZE];
    heap_timer* timer;
};

//定时器类
class heap_timer 
{
public:
    heap_timer(int delay) 
    {
        expire = time(nullptr) + delay;
    }
    time_t expire; //定时器生效的绝对时间
    void (*cb_func) (client_data*); //定时器的回调函数
    client_data* user_data; //用户数据
};

//时间堆类
class time_heap 
{
public:
    //构造函数之一，初始化一个大小为cap的空堆
    time_heap(int cap)// throw (std::exception) 
    : capacity(cap), cur_size(0)
    {
        array = new heap_timer* [capacity]; //创建堆数组
        if (!array) 
        {
            throw std::exception();
        }
        for (int i = 0; i < capacity; i++) 
        {
            array[i] = nullptr;
        }
    }

    //构造函数之二，用已有数组来初始化堆
    time_heap(heap_timer** init_arry, int size, int capacity)
    : cur_size(size), capacity(capacity) 
    {
        if (capacity < size) 
        {
            std::cout << "error\n";
        }
        array = new heap_timer* [capacity]; //创建数组
        if (!array) 
        {
            std::cout << "error\n";
        }
        for (int i = 0; i < capacity; i++) 
        {
            array[i] = nullptr;
        }
        if (size != 0) 
        {
            //初始化堆数组
            for (int i = 0; i < capacity; i++) 
            {
                array[i] = init_arry[i];
            }
            //i是第cur_size的父亲节点
            for (int i = (cur_size-1) / 2; i >= 0; i--) 
            {
                //对数组中的第[(cur_size-1)/2]~0个元素执行下虑操作
                percolate_down(i);
            }
        }
    }

    //销毁时间堆
    ~time_heap()
    {
        for (int i = 0; i < cur_size; i++) 
        {
            delete array[i];
        }
        delete[] array;
    }

    //添加目标定时器timer
    void add_timer(heap_timer* timer) 
    {
        if (!timer) 
        {
            return ;
        }
        if (cur_size >= capacity) //如果当前堆数组容量不够，则将其扩大一倍
        {
            resize();
        }

        //新插入了一个元素，当前堆大小加1，hole是新建空穴的位置
        int hole = cur_size++;
        int parent = 0;
        for (; hole > 0; hole = parent) 
        {
            parent = (hole-1) / 2;
            if (array[parent]->expire <= timer->expire) //如果插入的这个节点值大于父节点
            {
                break;
            }
            //否则交换父节点和该节点
            array[hole] = array[parent];
        }
        array[hole] = timer;
    }

    //删除目标定时器
    void del_timer(heap_timer* timer) 
    {
        if (!timer) 
        {
            return ;
        }

        //仅仅将目标定时器的回调函数设置为空，即所谓的延迟销毁。这将节省
        //真正删除该定时器造成的开销，但这样做容易使堆数组膨胀
        timer->cb_func = nullptr;
    }

    //获得堆顶部的定时器
    heap_timer* top() const 
    {
        if (empty()) 
        {
            return nullptr;
        }
        return array[0];
    }

    //删除堆顶部的定时器
    void pop_timer() 
    {
        if (empty()) 
        {
            return ;
        }
        if (array[0]) 
        {
            delete array[0];
            //将原来的堆顶元素替换为堆数组中最后一个元素
            //但这样做没有改变堆数组队后一个元素的指向，只是将数组
            //大小-1了，可能有点占用内存
            array[0] = array[--cur_size];
            percolate_down(0); //对新的堆顶元素执行下虑操作
        }
    }

    //心搏函数 
    void tick() 
    {
        heap_timer* tmp = array[0];
        time_t cur = time(nullptr); //循环处理堆中到期的定时器
        while (!empty()) 
        {
            if (!tmp) 
            {
                break;
            }
            //如果堆顶定时器没到期，退出循环
            if (tmp->expire > cur) 
            {
                break;
            }
            //否则执行堆顶中的定时器
            if (array[0]->cb_func) 
            {
                array[0]->cb_func(array[0]->user_data);
            }
            //将堆顶元素删除，同时生成新的堆顶定时器(array[0])
            pop_timer();
            tmp = array[0];
        }
    }

    bool empty() const 
    {
        return cur_size == 0;
    }
private:
    //下虑不是排序生成最小堆，是在一个已经是最小堆的基础上改变了一个节点，然后确
    //保其还是最小堆

    //最小堆的下虑操作，他确保堆数组中以第hole个节点作为根的子树拥有最小堆性质
    void percolate_down(int hole) 
    {
        //相当于tmp保存该节点，然后遍寻其子树，遇到比他小的，就将它指向那个孩子，
        //然后最后那个孩子指向tmp
        heap_timer* tmp = array[hole];
        int child = 0;

        //以数组构成的二叉树，任意位置i上的元素，左儿子在2*i+1上，右儿子在2*i+2上，父亲在(i-1)/2上
        for (; ((hole*2+1) <= (cur_size-1)); hole = child) 
        {
            child = hole * 2 + 1; //child现在是hole左儿子的位置
            if ((child < (cur_size-1)) && (array[child+1]->expire < array[child]->expire)) 
            { //还有右孩子且右孩子的值比左孩子小
                ++child;
            }
            if (array[child]->expire < tmp->expire) 
            { //左孩子（右孩子）的值小，改变数组hole位置的指向,指向孩子
                array[hole] = array[child];
            }
            else 
            {
                break;
            }
        }
        //将最后失去被指向的孩子的位置，指向tmp
        array[hole] = tmp;
    }

    //将堆数组容量扩大一倍
    void resize() //throw (std::exception);
    {
        heap_timer** tmp = new heap_timer* [2*capacity];
        for (int i = 0; i < cur_size; i++) 
        {
            tmp[i] = nullptr;
        }
        if (!tmp) 
        {
            std::cout << "resize error\n";
            return ;
        }
        capacity = 2 * capacity;
        for (int i = 0; i < capacity; i++) 
        {
            tmp[i] = array[i];
        }
        delete[] array;
        array = tmp;
    }

private:
    heap_timer** array; //堆数组
    int capacity; //堆数组的容量
    int cur_size; //堆数组当前包含元素的个数
};
#endif
