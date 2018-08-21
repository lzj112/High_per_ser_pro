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

#define PORT 4096
#define IP "127.0.0.1" 

int main() 
{
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    inet_pton(AF_INET, IP, &address.sin_addr);
    address.sin_port = htons(PORT);
    address.sin_family = AF_INET;

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);

    int ret = bind(sockfd, &address, sizeof(address));
    assert(ret != -1);

    ret = listen(sockfd, 5);
    assert(ret != -1);

    struct sockaddr_in clientaddress;
    bzero(&clientaddress, sizeof(clientaddress));
    socklen_t len = sizeof(clientaddress);
    int connfd = accept(sockfd, &clientaddress, &len);

    char buf[20];
    recv(connfd, buf, sizeof(buf), 0);
    printf("recv from client :\n%s\n", buf);

    bzero(buf, sizeof(buf));
    strcpy(buf, "this is right!");
    send(connfd, buf, sizeof(buf), 0);

    close(connfd);

    return 0;
}