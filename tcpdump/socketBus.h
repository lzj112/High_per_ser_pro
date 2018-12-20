#ifndef _SOCKETBUS_H
#define _SOCKETBUS_H

#include "socketTCP.h"

class socketBus : public socketTCP 
{
public:
    void startListening(const char* ip, int port);
    int sendTo(const char* str, int connfd = -1); 
    int makeNewConn(const char* ip, int port);
    void recvFrom(void* buf, int length);
};  

#endif