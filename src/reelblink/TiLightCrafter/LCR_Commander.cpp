#include "LCR_Commander.h"

LCR_Command::LCR_Command( uint8 packetType, uint16 commandId, uint8 flags ) :
m_payloadLength( 0 )
{
  m_payLoad = unique_ptr<uint8[]>( new uint8[MAX_PACKET_SIZE] );
  // Initialize the command header
  m_payLoad[0] = packetType;
  m_payLoad[1] = (commandId >> 8) & 0xFF;
  m_payLoad[2] = commandId & 0xFF;
  m_payLoad[3] = flags;
}

unsigned long LCR_Command::AppendPayload( uint8* payload, unsigned long payLoadLength )
{
  unsigned long appended = m_payloadLength + payLoadLength < MAX_PAYLOAD_SIZE ?
	payLoadLength : MAX_PAYLOAD_SIZE - m_payloadLength;

  // Insert the payload after any existing payload
  for( unsigned int i = 0; i < appended; i++)
	{ m_payLoad[HEADER_SIZE + m_payloadLength + i] = payload[i]; }
  m_payloadLength += appended;

  return appended;
}

unsigned long LCR_Command::GetCommandSize( void )
{ return HEADER_SIZE + m_payloadLength + CHECKSUM_SIZE; }

uint8* LCR_Command::GetCommand( void )
{
  // Set the correct length and calculate the checksum
  m_payLoad[4] = m_payloadLength & 0xFF;
  m_payLoad[5] = ( m_payloadLength >> 8 ) & 0xFF;

  // Append the checksum
  int sum = 0;
  for(int i = 0; i < m_payloadLength; i++)
	{ sum += m_payLoad[i]; }
  m_payLoad[HEADER_SIZE + m_payloadLength] = (uint8)(sum & 0xFF);

  return m_payLoad.get( );
}

LCR_Commander::LCR_Commander()
{
  tcpClient = unique_ptr<Tcp>( new Tcp( ) );
  connectedSocket = -1;
}

bool LCR_Commander::Connect_LCR(string ipAddress, string port)
{
  for(int i = 0; i < ConnectionAttempts; ++i)
  { 
	SOCKET socket = tcpClient->TCP_Connect(ipAddress, port);
	if(socket > 0)
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
  if(connectedSocket > 0)
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

  // Send the Packet	
  if(SOCKET_ERROR == tcpClient->TCP_Send( connectedSocket, command, packetSize ) )
  {
	cout << "PACKET SEND, NUMBER: " << packetNumber 
		 << " with length: "<< packetSize
		 <<	" has recieved a socket error.\n";
	return false;
  }


  //-----------Recieve  Packet----------------------------
  uint8 recieve[HEADER_SIZE];
  if(SOCKET_ERROR == tcpClient->TCP_Receive(connectedSocket,recieve,HEADER_SIZE))
  {
	cout << "PACKET Recieve Header, NUMBER: " << packetNumber
		 <<" with length: " << packetSize
		 <<" has recieved a socket error.\n";
	return false;
  }

  int payLoadLength = recieve[5] << 8 + recieve[4]; // the 5th is the msb and the 4th is the lsb of the payload Length
  int sizeToRecieve = payLoadLength+CHECKSUM_SIZE;

  unique_ptr<uint8[]> recievePayLoad(new uint8[sizeToRecieve]);

  if(SOCKET_ERROR == tcpClient->TCP_Receive(connectedSocket,recievePayLoad.get(),sizeToRecieve))
  {
	cout << "PACKET Recieve PayLoad, NUMBER: " << packetNumber
		 << " with length: " << packetSize 
		 << " has recieved a socket error.\n";
	return false;
  }

  return true;
}

bool LCR_Commander::ProjectImage( uint8* image, int byteCount )
{
  // If there is no connected socket we cannot communicate with the projector
  LC_CHECKRETURNLOGERROR( ( 0 < connectedSocket ), "Not connected to light crafter" )
  
  //------------- First Packet----------------------------------------------
  LCR_Command command( Host_Write, StaticImage, Beginning );
  command.AppendPayload( image );

  LC_CHECKRETURNLOGERROR( 
	SendLCRWriteCommand( command.GetCommand( ), command.GetCommandSize( ), 1), 
	"Unable to issue command to light crafter" )

  //-------------Intermediate Packets---------------------------------------------
  // Calculate the number of intermediate packets that we will be sending
  //BYTE count divided by max packet size +1 - 2 intermediate packets, should be 37
  int numberOfIntermediatePackets = (byteCount/MAX_PAYLOAD_SIZE+1)-2;  

  for(int i = 0; i < numberOfIntermediatePackets; i++)
  {
	auto commandIntermediate = CreateCommand((uint8)  Host_Write, 
                                             (uint16) StaticImage, 
                                             (uint8)  Intermediate, 
                                                      MAX_PAYLOAD_SIZE, 
                                                      image + (MAX_PAYLOAD_SIZE) * (i+1));

	LC_CHECKRETURNLOGERROR( 
	  SendLCRWriteCommand(commandIntermediate.get(), MAX_PACKET_SIZE, i+1), 
	  "Failed to issue intermediate command" ); 
  }


  //-------------Final Packet---------------------------------------------
  /* The last packet does not contain the max payload size, to get the remaining bytes, take the total bytes of the image
  minus the max payload multipled by the number of packets being sent, which is intermedatepacket count + first and last,
  this will get you a neg number which is the remaing size*/

  int remainingBytes = -(byteCount - ((MAX_PAYLOAD_SIZE)*(numberOfIntermediatePackets+2)));
  auto commandFinal = CreateCommand( (uint8)  Host_Write, 
	                                 (uint16) StaticImage, 
                                     (uint8)  End, 
                                              remainingBytes, 
                                              image + MAX_PAYLOAD_SIZE * numberOfIntermediatePackets);

  int lastLength = remainingBytes + HEADER_SIZE + CHECKSUM_SIZE;
  LC_CHECKRETURNLOGERROR(
	SendLCRWriteCommand(commandFinal.get(), lastLength, numberOfIntermediatePackets+2),
	"Unable to issue final command to light crafter" );

  return true;
}

bool LCR_Commander::LCR_LOAD_STATIC_IMAGE(uint8 * image,int byteCount)
{
  // If there is no connected socket we cannot communicate with the projector
  if(connectedSocket < 0)
  {
	cout << "No connected Socket.\n";
	return false;
  }
  
  //------------- First Packet----------------------------------------------
  auto commandHeader = CreateCommand( (uint8)  Host_Write, 
									  (uint16) StaticImage, 
                                      (uint8)  Beginning, 
                                               MAX_PAYLOAD_SIZE, 
                                               image);

  // Send the first Packet - If unable, return false to indicate an error
  if( !SendLCRWriteCommand( commandHeader.get(), MAX_PACKET_SIZE, 1) )
	{ return false; }

  //-------------Intermediate Packets---------------------------------------------
  // Calculate the number of intermediate packets that we will be sending
  //BYTE count divided by max packet size +1 - 2 intermediate packets, should be 37
  int numberOfIntermediatePackets = (byteCount/MAX_PAYLOAD_SIZE+1)-2;  

  for(int i = 0; i < numberOfIntermediatePackets; i++)
  {
	auto commandIntermediate = CreateCommand((uint8)  Host_Write, 
																 (uint16) StaticImage, 
																 (uint8)  Intermediate, 
																          MAX_PAYLOAD_SIZE, 
																		  image + (MAX_PAYLOAD_SIZE) * (i+1));

	// If we fail to send the packet, return false to indicate an error
    if(!SendLCRWriteCommand(commandIntermediate.get(), MAX_PACKET_SIZE, i+1))
	  { return false; }
  }


  //-------------Final Packet---------------------------------------------
  /* The last packet does not contain the max payload size, to get the remaining bytes, take the total bytes of the image
  minus the max payload multipled by the number of packets being sent, which is intermedatepacket count + first and last,
  this will get you a neg number which is the remaing size*/

  int remainingBytes = -(byteCount - ((MAX_PAYLOAD_SIZE)*(numberOfIntermediatePackets+2)));

  auto commandFinal = CreateCommand((uint8)  Host_Write, 
	                                                    (uint16) StaticImage, 
														(uint8)  End, 
														         remainingBytes, 
																 image + MAX_PAYLOAD_SIZE * numberOfIntermediatePackets);

  int lastLength = remainingBytes + HEADER_SIZE + CHECKSUM_SIZE;

  return SendLCRWriteCommand(commandFinal.get(), lastLength, numberOfIntermediatePackets+2);
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
  uint8* payLoad;
  payLoad = &dplMode;

  uint16 payLoadLength = 0x1;

  //create the command
  auto command = CreateCommand((uint8) pType, (uint16) cmdId, (uint8) flag, payLoadLength, payLoad);

  int totalLength = HEADER_SIZE + payLoadLength + CHECKSUM_SIZE;

  bool sendResult = SendLCRWriteCommand(command.get(),totalLength);

  if(sendResult == false)
  { cout <<"LCR Set Display mode send has send error.\n"; }	

  return sendResult;
}

unique_ptr<uint8[]> LCR_Commander::CreateCommand( uint8 packetType, uint16 commandId, uint8 flags, long payLoadLength, uint8* payLoad )
{
  int size = HEADER_SIZE + payLoadLength + CHECKSUM_SIZE;
  unique_ptr<uint8[]> command(new uint8[size]);

  // Initialize the command header
  command[0] = packetType;
  command[1] = (commandId >> 8) & 0xFF;
  command[2] = commandId & 0xFF;
  command[3] = flags;
  command[4] = payLoadLength & 0xFF;
  command[5] = (payLoadLength >> 8) & 0xFF;

  // Insert the payload
  for( int i = 0; i < payLoadLength; i++)
	{ command[6+i] = payLoad[i]; }
 
  // Append the checksum
  int sum = 0;
  for(int i = 0; i < size; i++)
  {
    sum += command[i];
  }
  command[size-1] = (uint8)(sum & 0xFF);
	
  return command;
}