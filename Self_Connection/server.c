#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

#define IP "127.0.0.1"
#define PORT 5001

int main(int argc, char* argv[]) 
{
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &address.sin_addr);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);
    
    int optval = 1;
    int ret = setsockopt(sock,
                           SOL_SOCKET,
                           SO_REUSEADDR,
                           &optval,
                           sizeof(optval));
    assert(ret != -1);

    ret = bind(sock, 
               (struct sockaddr*)&address, 
               sizeof(address));
    assert(ret != -1);

    ret = listen(sock, 5);
    assert(ret != -1);

    struct sockaddr_in cli;
    socklen_t cliLen = sizeof(cli);
    int connfd = accept(sock,
                        (struct sockaddr *)&cli,
                        &cliLen);

    printf("connfd = %d\n", connfd);

   return 0;
}