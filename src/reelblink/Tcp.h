#ifndef _TCP_H_
#define _TCP_H_

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#include "LCR_Common.h"

using namespace std;

class Tcp
{
private:
  SOCKET m_socket;

public:
  Tcp( void );

  bool Connect( string ipAddress, string port );
  bool Disconnect( void );

  int  Send( unsigned char *buffer, int length );
  int  Receive( unsigned char *buffer, int length );
  
  bool Connected( void );
};

#endif