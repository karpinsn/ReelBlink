#ifndef _LCR_COMMANDER_H_
#define _LCR_COMMANDER_H_

using namespace std;
#include <string>
#include <cmath>
#include <stdio.h>
#include <math.h>
#include <memory>

#include "Tcp.h"
#include "LCR_Common.h"
#include "LCR_Command_Packetizer.h"


#include <iostream>


#define ConnectionAttempts 5

class LCR_Command
{
private:
  unsigned long m_payloadLength;
  unique_ptr<uint8[]> m_payLoad;

public:
  LCR_Command( uint8 packetType, uint16 commandId, uint8 flags );
  unsigned long AppendPayload( uint8* payload, unsigned long payLoadLength = MAX_PAYLOAD_SIZE );

  unsigned long GetCommandSize( void );
  uint8* GetCommand( void );
};

class LCR_Commander 
{
private:

    unique_ptr<Tcp> tcpClient;
	
	SOCKET connectedSocket;

	bool SendLCRWriteCommand(uint8* command, long packetSize, int packetNumber = -1);

public:
	LCR_Commander();

	bool Connect_LCR(string ipAddress, string port);
	bool Disconnect_LCR(void);

	bool SetDisplayMode(DisplayMode displayMode);
	bool ProjectImage(uint8 * image,int byteCount);
	bool LCR_LOAD_STATIC_IMAGE(uint8 * image,int byteCount);

private:
  unique_ptr<uint8[]> CreateCommand( uint8 packetType, uint16 commandId, uint8 flags, long payLoadLength, uint8* payLoad );
};

#define LC_CHECKRETURNLOGERROR( COMMAND, ERROR ) \
{ \
  if( !COMMAND ) \
	{ cout << ERROR << endl; return false; } \
}

#endif