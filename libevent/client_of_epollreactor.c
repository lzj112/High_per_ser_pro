#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <errno.h>

#define PORT 8080
#define IP "127.0.0.1"

int main() 
{
    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &server.sin_addr);

    int ret = 0;
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);

    ret = connect(sockfd, (struct sockaddr*)&server, sizeof(server));
    assert(ret >= 0);

    char buf[] = "helloe world!";
    
    while (1)
    {
        send(sockfd, buf, sizeof(buf), 0);
        printf("sending\n");
        sleep(1);
    }
    //assert(ret > 0);
}
