#ifndef _LCR_COMMANDER_H_
#define _LCR_COMMANDER_H_

using namespace std;
#include <string>
#include <cmath>
#include <stdio.h>
#include <math.h>

#include "Tcp.h"
#include "LCR_Common.h"
#include "LCR_Command_Packetizer.h"


#include <iostream>


#define ConnectionAttempts 5

class LCR_Commander 
{
private:
	Tcp * tcpClient;
	int connectedSocket;
	Command_Packetizer* packetizer;

	bool LCR_Commander::SendLCRWriteCommand(uint8* command, long packetSize, int packetNumber = -1);

public:
	LCR_Commander(void);
	~LCR_Commander(void);

	bool Connect_LCR(string ipAddress, string port);
	bool Disconnect_LCR(void);

	bool SetDisplayMode(DisplayMode displayMode);

	bool LCR_LOAD_STATIC_IMAGE(uint8 * image,int byteCount);
};


#endif