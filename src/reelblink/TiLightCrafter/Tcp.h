
#ifndef _TCP_H_
#define _TCP_H_



#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>

#include "LCR_Common.h"



// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define TRUE                    1
#define FALSE                   0

using namespace std;

class Tcp
{

public:
	int TCP_Connect(string ipAddress, string port);
    int TCP_Send(int sock,  unsigned char *buffer, int length);
    int TCP_Receive(int sock, unsigned char *buffer, int length);
    int TCP_Disconnect(int sock);

};

#endif