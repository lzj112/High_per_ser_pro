# 判断自连接

## 很简单的两个示例:

- server.c

使用阻塞套接字 accept,有新的连接接收并打印返回的套接字

- client.c

使用阻塞套接字 connect, 在成功后再次调用 connect 进行判断, 如果连接成功无误, 会返回 EISCONN

因为自连接不被认为是 TCP 协议实现中的 bug, 所以不管是正常连接还是自连接, 再次调用 connect 都会返回 EISCONN, 我们再输出服务端和客户端的 IP port 进行比对
