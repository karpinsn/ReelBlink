#include "LCR_Commander.h"

LCR_Commander::LCR_Commander(void)
{
	tcpClient = new Tcp();
	connectedSocket = -1;

	packetizer = new Command_Packetizer();
}

LCR_Commander::~LCR_Commander(void)
{
	if(tcpClient != NULL)
		delete tcpClient;

	if(packetizer != NULL)
		delete packetizer;


}

bool LCR_Commander:: Connect_LCR(string ipAddress, string port)
{
	for(int i = 0; i<ConnectionAttempts; ++i)
	{ 
		int socket = tcpClient->TCP_Connect(ipAddress, port);
		if(socket >0)
		{
			connectedSocket = socket;
			return true;
		}
	}

	return false;

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
			cout <<"PACKET SEND, NUMBER:"<<packetNumber<<" with length:"<<packetSize<<" has recieved a socket error.\n";
		else
			cout <<"PACKET SEND, with length:"<<packetSize<<" has recieved a socket error.\n";
		return false;
	}


	//-----------Recieve  Packet----------------------------
	uint8 recieve[HEADER_SIZE];
	int rec = tcpClient->TCP_Receive(connectedSocket,recieve,HEADER_SIZE);
	if(rec ==SOCKET_ERROR)
	{
		if(packetNumber != -1)
			cout <<"PACKET Recieve Header, NUMBER:"<<packetNumber<<" with length:"<<packetSize<<" has recieved a socket error.\n";
		else
			cout <<"PACKET Recieve Header, with length:"<<packetSize<<" has recieved a socket error.\n";
		return false;
	}

	int payLoadLength = recieve[5]<<8+recieve[4];
	int sizeToRecieve = payLoadLength+CHECKSUM_SIZE;
	
	uint8* recievePayLoad = new uint8[sizeToRecieve];
	int recPayload = tcpClient->TCP_Receive(connectedSocket,recievePayLoad,sizeToRecieve);
	if(recPayload ==SOCKET_ERROR)
	{
		if(packetNumber != -1)
			cout <<"PACKET Recieve PayLoad, NUMBER:"<<packetNumber<<" with length:"<<packetSize<<" has recieved a socket error.\n";
		else
			cout <<"PACKET Recieve PayLoad, with length:"<<packetSize<<" has recieved a socket error.\n";
		return false;
	}
	delete[] recievePayLoad;

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
	uint8* commandHeader = packetizer->CreateCommand((uint8) pType, (uint16) cmdId, (uint8) flag, MAX_PAYLOAD_SIZE, image);

	//send the first Packet
	bool sendFirst = SendLCRWriteCommand(commandHeader,MAX_PACKET_SIZE,1);
	
	delete[] commandHeader;
	
	if(!sendFirst)
	{
		return false;
	}

	//-------------Intermediate Packets---------------------------------------------
	flag = Intermediate; // change the flag to intermediate

	int NumberOfIntermediatePackets = 37;

	for(int i = 0;i<NumberOfIntermediatePackets;i++)
	{
	  uint8* commandIntermediate = packetizer->CreateCommand((uint8) pType, (uint16) cmdId, (uint8) flag, MAX_PAYLOAD_SIZE, image+(MAX_PAYLOAD_SIZE)*(i+1));

      bool sendIntermdiate = SendLCRWriteCommand(commandIntermediate,MAX_PACKET_SIZE,i+1);
	  delete[] commandIntermediate;

	  if(!sendIntermdiate)
	  {
	     return false;
	  }
	  
	}


	//-------------Final Packet---------------------------------------------

	flag = End; // change the flag to indicate last packet

	int remainingBytes = -(byteCount - ((MAX_PAYLOAD_SIZE)*(NumberOfIntermediatePackets+2)));

	uint8* commandFinal = packetizer->CreateCommand((uint8) pType, (uint16) cmdId, (uint8) flag, remainingBytes, image+MAX_PAYLOAD_SIZE*NumberOfIntermediatePackets);

	int lastLength = remainingBytes +HEADER_SIZE+CHECKSUM_SIZE;

	 bool sendFinal = SendLCRWriteCommand(commandFinal,lastLength,NumberOfIntermediatePackets+2);
	 
	 if(!sendFinal)
	  {
	     return false;
	  }


	return true;
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
	uint8* command = packetizer->CreateCommand((uint8) pType, (uint16) cmdId, (uint8) flag, payLoadLength, payLoad);


	int totalLength = HEADER_SIZE + payLoadLength + CHECKSUM_SIZE;
	int sendResult = tcpClient->TCP_Send(connectedSocket,command,totalLength);

	delete[] command;

	if(sendResult ==SOCKET_ERROR)
	{
		cout <<"LCR Set Display mode send has send error.\n";

		return false;
	}

	return true;
}

bool LCR_Commander::Disconnect_LCR(void)
{
	if(connectedSocket >0)
	{
		int status = tcpClient->TCP_Disconnect(connectedSocket);

		if(status == 0)
		{
		  connectedSocket = -1;
		  return true;
		}

		else
		  return false;
	}
	else
	  return false;


}
