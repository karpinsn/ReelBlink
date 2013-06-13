#ifndef _LCR_COMMAND_PACKETIZER_H_
#define _LCR_COMMAND_PACKETIZER_H_



#include <stdio.h>
#include <string.h>
#include <memory>
#include "LCR_Common.h"


using namespace std;

class Command_Packetizer
{
private:

	static void InitializeCommandBuffer(uint8* command, uint8 packetType, uint16 commandId, uint8 flags, long payLoadLength);
	static void LoadPayLoadInBuffer (uint8 * command, uint8* payLoad, long payLoadLength);
	static void CalculateCheckSum(uint8* command, long size);

public:
	
	static unique_ptr<uint8[]> CreateCommand(uint8 packetType, uint16 commandId, uint8 flags, long payLoadLength, uint8* payLoad);

};


#endif

