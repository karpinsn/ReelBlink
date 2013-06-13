
#ifndef _TCP_H_
#define _TCP_H_



#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>

#include "LCR_Common.h"


using namespace std;

class Tcp
{

public:
	SOCKET TCP_Connect(string ipAddress, string port);
    int TCP_Send(int sock,  unsigned char *buffer, int length);
    int TCP_Receive(int sock, unsigned char *buffer, int length);
    bool TCP_Disconnect(int sock);

};

#endif