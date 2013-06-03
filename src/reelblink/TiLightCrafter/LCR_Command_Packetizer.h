#ifndef _LCR_COMMAND_PACKETIZER_H_
#define _LCR_COMMAND_PACKETIZER_H_



#include <stdio.h>
#include <string.h>
#include "LCR_Common.h"

using namespace std;

class Command_Packetizer
{
private:
	void InitilizeCommandBuffer(uint8* command, uint8 packetType, uint16 commandId, uint8 flags, long payLoadLength);
	void LoadPayLoadInBuffer (uint8 * command, uint8* payLoad, long payLoadLength);
	void CalculateCheckSum(uint8* command, long size);

public:
	Command_Packetizer(void);
	~Command_Packetizer(void);

	uint8* CreateCommand(uint8 packetType, uint16 commandId, uint8 flags, long payLoadLength, uint8* payLoad);

};


#endif

