#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "socketTCP.h"

int port = 5000;
char* ip = "127.0.0.1";

int main() 
{
    socketTCP sock;
    sock.initSocketTCP();
    sock.Bind(ip, port);
    sock.Listen();

    int connfd = sock.Accept();

    char buffer[250];
    while (1) 
    {
        memset(buffer, 0, 250);
        int res = recv(connfd, buffer, sizeof(buffer), 0);
        if (res > 0)
        printf("recv from %d == %s\n", connfd, buffer);
    }
}