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

    if (connect(sockfd, (struct sockaddr*)&address, sizeof(address)) < 0) 
    {
        printf("connection failed\n");
    }
    else 
    {
        char data[20] = "Could this pooible?";
        send(sockfd, data, sizeof(data), 0);
        close(sockfd);
    }
    char buf[20];
    recv(sockfd, buf, sizeof(buf), 0);
    printf("after close : \n%s\n", buf);
    // close(sockfd);
    return 0;
}