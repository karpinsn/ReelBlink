#include "LCR_Commander.h"

LCR_Commander::LCR_Commander()
{
	tcpClient = unique_ptr<Tcp>(new Tcp());
	connectedSocket = -1;
}

bool LCR_Commander:: Connect_LCR(string ipAddress, string port)
{
	for(int i = 0; i<ConnectionAttempts; ++i)
	{ 
		SOCKET socket = tcpClient->TCP_Connect(ipAddress, port);
		if(socket >0)
		{
			connectedSocket = socket;
			return true;
		}
	}

	return false;
}

bool LCR_Commander::Disconnect_LCR(void)
{
	bool disconnect = false;
	if(connectedSocket >0)
	{
		disconnect = tcpClient->TCP_Disconnect(connectedSocket);

		if(disconnect)
		{
		  connectedSocket = -1;
		 
		}

	}
	return disconnect;
}


bool LCR_Commander::SendLCRWriteCommand(uint8* command, long packetSize, int packetNumber)
{
	if(connectedSocket<0)
	{
		cout << "No connected Socket.\n";
		return false;
	}

	//send the  Packet
	int send = tcpClient->TCP_Send(connectedSocket,command,packetSize);
	
	if(send ==SOCKET_ERROR)
	{
		if(packetNumber != -1)
		{
		  cout <<"PACKET SEND, NUMBER:"<<packetNumber<<" with length:"<<packetSize<<" has recieved a socket error.\n";
		}
			else
		{
		  cout <<"PACKET SEND, with length:"<<packetSize<<" has recieved a socket error.\n";
		}
		return false;
	}


	//-----------Recieve  Packet----------------------------
	uint8 recieve[HEADER_SIZE];
	int rec = tcpClient->TCP_Receive(connectedSocket,recieve,HEADER_SIZE);
	if(rec ==SOCKET_ERROR)
	{
		if(packetNumber != -1)
		{
			cout <<"PACKET Recieve Header, NUMBER:"<<packetNumber<<" with length:"<<packetSize<<" has recieved a socket error.\n";
		}
		else
		{
			cout <<"PACKET Recieve Header, with length:"<<packetSize<<" has recieved a socket error.\n";
		}
		return false;
	}

	int payLoadLength = recieve[5]<<8+recieve[4]; // the 5th is the msb and the 4th is the lsb of the payload Length
	
	
	int sizeToRecieve = payLoadLength+CHECKSUM_SIZE;

	unique_ptr<uint8[]> recievePayLoad(new uint8[sizeToRecieve]);

	int recPayload = tcpClient->TCP_Receive(connectedSocket,recievePayLoad.get(),sizeToRecieve);
	if(recPayload ==SOCKET_ERROR)
	{
		if(packetNumber != -1)
		{			
			cout <<"PACKET Recieve PayLoad, NUMBER:"<<packetNumber<<" with length:"<<packetSize<<" has recieved a socket error.\n";
		}
		else
		{
			cout <<"PACKET Recieve PayLoad, with length:"<<packetSize<<" has recieved a socket error.\n";
		}
		return false;
	}

	return true;

}


bool LCR_Commander::LCR_LOAD_STATIC_IMAGE(uint8 * image,int byteCount)
{
	if(connectedSocket < 0)
	{
		cout << "No connected Socket.\n";
		return false;
	}

	Packet_Type pType = Host_Write; // read command 
	CommandIds cmdId = StaticImage; //commandId
	Command_Flags flag = Beginning; //flags


			//-------------First Packet----------------------------------------------
	auto commandHeader = Command_Packetizer::CreateCommand((uint8) pType, (uint16) cmdId, (uint8) flag, MAX_PAYLOAD_SIZE, image);

	//send the first Packet
	bool sendFirst = SendLCRWriteCommand(commandHeader.get(),MAX_PACKET_SIZE,1);
	
	
	if(!sendFirst)
	{
		return false;
	}

	//-------------Intermediate Packets---------------------------------------------
	flag = Intermediate; // change the flag to intermediate

	int NumberOfIntermediatePackets = (byteCount/MAX_PAYLOAD_SIZE+1)-2;  //BYTE count divided by max packet size +1 - 2 intermediate packets, should be 37

	for(int i = 0;i<NumberOfIntermediatePackets;i++)
	{
	  auto commandIntermediate = Command_Packetizer::CreateCommand((uint8) pType, (uint16) cmdId, (uint8) flag, MAX_PAYLOAD_SIZE, image+(MAX_PAYLOAD_SIZE)*(i+1));

      if(!SendLCRWriteCommand(commandIntermediate.get(),MAX_PACKET_SIZE,i+1))
      {
      	return false;
      }
	  
	}


	//-------------Final Packet---------------------------------------------

	flag = End; // change the flag to indicate last packet

	/* The last packet does not contain the max payload size, to get the remaining bytes, take the total bytes of the image
	minus the max payload multipled by the number of packets being sent, which is intermedatepacket count + first and last,
	this will get you a neg number which is the remaing size*/

	int remainingBytes = -(byteCount - ((MAX_PAYLOAD_SIZE)*(NumberOfIntermediatePackets+2)));


	auto commandFinal = Command_Packetizer::CreateCommand((uint8) pType, (uint16) cmdId, (uint8) flag, remainingBytes, image+MAX_PAYLOAD_SIZE*NumberOfIntermediatePackets);

	int lastLength = remainingBytes +HEADER_SIZE+CHECKSUM_SIZE;

	bool sendFinal = SendLCRWriteCommand(commandFinal.get(),lastLength,NumberOfIntermediatePackets+2);
	 
	return sendFinal;
}

bool LCR_Commander::SetDisplayMode(DisplayMode displayMode)
{
	if(connectedSocket < 0)
	{
      cout << "No connected Socket.\n";
	  return false;
	}


	Packet_Type pType = Host_Write; // read command
	CommandIds cmdId = CurrentDisplayMode; //commandId
	Command_Flags flag = DataComplete; //flags


	uint8 dplMode = (uint8)displayMode; // set the display mode
	uint8 * payLoad;
	payLoad = &dplMode;

	uint16 payLoadLength = 0x1;

	//create the command
	auto command = Command_Packetizer::CreateCommand((uint8) pType, (uint16) cmdId, (uint8) flag, payLoadLength, payLoad);

	int totalLength = HEADER_SIZE + payLoadLength + CHECKSUM_SIZE;

    bool sendResult = SendLCRWriteCommand(command.get(),totalLength);

	if(sendResult == false)
	{
		cout <<"LCR Set Display mode send has send error.\n";
	}	

	return sendResult;
}


