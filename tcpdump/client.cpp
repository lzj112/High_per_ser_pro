#include <stdio.h>
#include <unistd.h>
#include "socketTCP.h"


char* ip = "127.0.0.1";
int port = 5000;
int main() 
{
    socketTCP sock;
    sock.initSocketTCP();
    sock.Connect(ip, port);

    int fd = sock.getMysockTCP();
    while (1) 
    {
        char buffer[] = "i`m lvbai. fuck u!";
        send(fd, buffer, sizeof(buffer), 0);
        sleep(1);
    }
}