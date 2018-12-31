# 自连接

客户端一般不 bind, 这时候由内核随机分配一个 port, 但是如果这个时候正好分配的那个 port 就是你想 connect 的 port, 就会发生自连接, 而且认为连接成功


# 判断自连接

## 很简单的两个示例:

- server.c

使用阻塞套接字 accept,有新的连接接收并打印返回的套接字

- client.c

使用阻塞套接字 connect, 在成功后再次调用 connect 进行判断, 如果连接成功无误, 会返回 EISCONN

因为自连接不被认为是 TCP 协议实现中的 bug, 所以不管是正常连接还是自连接, 再次调用 connect 都会返回 EISCONN, 我们再输出服务端和客户端的 IP port 进行比对

# 如何预防自连接

客户端默认分配的端口号在 `/proc/sys/net/ipv4/ip_local_port_range`, 只需要让服务器避开, 比如这个范围默认最小是从`32768`开始, 让服务器 bind 一个低于`32768`的 port
