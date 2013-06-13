#include "LCR_Command_Packetizer.h"


unique_ptr<uint8[]> Command_Packetizer::CreateCommand(uint8 packetType, uint16 commandId, uint8 flags, long payLoadLength, uint8* payLoad)
{
  int size = HEADER_SIZE + payLoadLength + CHECKSUM_SIZE;
  unique_ptr<uint8[]> command(new uint8[size]);

  InitializeCommandBuffer(command.get(), packetType, commandId, flags, payLoadLength);
  LoadPayLoadInBuffer(command.get(), payLoad, payLoadLength );
  CalculateCheckSum(command.get(),size-1);
	
  return command;
}


void Command_Packetizer::InitializeCommandBuffer(uint8* command, uint8 packetType, uint16 commandId, uint8 flags, long payLoadLength)
{    
  command[0] = packetType;
  command[1] = (commandId >> 8) & 0xFF;
  command[2] = commandId & 0xFF;
  command[3] = flags;
  command[4] = payLoadLength & 0xFF;
  command[5] = (payLoadLength >> 8) & 0xFF;
}

void Command_Packetizer::LoadPayLoadInBuffer(uint8* command, uint8* payLoad, long payLoadLength)
{
  if(command == nullptr)
	{ return; }
		
  for( int i = 0; i < payLoadLength; i++)
  {
	  command[6+i] = payLoad[i];
  }
}	

void Command_Packetizer::CalculateCheckSum(uint8* command, long size)
{
  int sum = 0;

  for(int i = 0; i < size; i++)
  {
    sum += command[i];
  }
  command[size] = (uint8)(sum & 0xFF);
}
