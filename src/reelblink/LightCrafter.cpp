#include "LightCrafter.h"

LightCrafter::LightCrafter( )
{
  m_tcpClient = unique_ptr<Tcp>( new Tcp( ) );
  
  Connect();
  //PatternDisplayMode( );
  StaticDisplayMode( );
}

bool LightCrafter::Connect()
{
  bool connected = false;
  for(int i = 0; !connected && i < ConnectionAttempts; ++i)
  { 
	connected = m_tcpClient->Connect( LCR_Default_IP, LCR_Default_PORT );
  }

  return connected;
}

bool LightCrafter::Disconnect()
  { return m_tcpClient->Disconnect( ); }

bool LightCrafter::PatternDisplayMode( )
{
  // If there is no connected socket we cannot communicate with the projector
  LC_CHECKRETURNLOGERROR( m_tcpClient->Connected( ), "Not connected to light crafter" );

  LCR_Command displayModeCommand( Host_Write, CurrentDisplayMode, DataComplete );
  uint8 payload = (uint8)PatternSequenceDisplay; // set the display mode
  displayModeCommand.AppendPayload( &payload, 1 );
  LC_CHECKRETURNLOGERROR( SendLCRCommand( displayModeCommand ), "LCR Set Display mode send has send error" );

  return true;
}

bool LightCrafter::StaticDisplayMode()
{
  // If there is no connected socket we cannot communicate with the projector
  LC_CHECKRETURNLOGERROR( m_tcpClient->Connected( ), "Not connected to light crafter" );

  LCR_Command displayModeCommand( Host_Write, CurrentDisplayMode, DataComplete );
  uint8 payload = (uint8)StaticImageMode; // set the display mode
  displayModeCommand.AppendPayload( &payload, 1 );
  LC_CHECKRETURNLOGERROR( SendLCRCommand( displayModeCommand ), "LCR Set Display mode send has send error" );

  return true;
}


bool LightCrafter::ProjectImage(cv::Mat image)
{
  // If there is no connected socket we cannot communicate with the projector
  LC_CHECKRETURNLOGERROR( m_tcpClient->Connected( ), "Not connected to light crafter" );

  // Step 1 - Get the image into the LCR format
  static const int parameters[] = { CV_IMWRITE_PXM_BINARY, 1, 0 };
  CvMat* imageStream;
  if(1 == image.channels())
  {
	cout << "Converting from single channel to triple channel image." << endl;
	cv::Mat convertedImage;
	cv::cvtColor( image, convertedImage, CV_GRAY2RGB );
	
	// Have to cast to old C version since we are using a c function
	imageStream = cvEncodeImage( ".bmp", &( ( CvMat ) convertedImage ), parameters );
  }
  else
	{ imageStream = cvEncodeImage( ".bmp", &( ( CvMat ) image ), parameters ); }

  // Step 2 - Packatize and stream
  unsigned long offset = 0;
  unsigned long byteCount = imageStream->step;
  Command_Flags flags = Beginning;
  while( offset < byteCount )
  {
	LCR_Command command( Host_Write, StaticImage, flags );
	offset += command.AppendPayload( imageStream->data.ptr + offset, byteCount - offset );

	flags = Intermediate;	  // After the first time through we send intermediate packets
	if( offset >= byteCount ) // Check to see if we hit the end, if so signify in the flags
	  { command.SetPacketType( End ); }

	LC_CHECKRETURNLOGERROR( SendLCRCommand( command ), "Unable to issue command to light crafter" );
  }
  
  return true;
}

bool LightCrafter::SendLCRCommand( LCR_Command& command )
{
  // If there is no connected socket we cannot communicate with the projector
  LC_CHECKRETURNLOGERROR( m_tcpClient->Connected( ), "Not connected to light crafter" );

  LC_CHECKRETURNLOGERROR( 
	( SOCKET_ERROR != m_tcpClient->Send( command.GetCommand( ), command.GetCommandSize( ) ) ),
	"Packet transmission error" );

  //-----------Recieve  Packet----------------------------
  uint8 recieve[HEADER_SIZE];
  LC_CHECKRETURNLOGERROR( 
	( SOCKET_ERROR != m_tcpClient->Receive( recieve, HEADER_SIZE ) ),
	"Packet receiving error" );

  int payLoadLength = ( recieve[5] << 8 ) + recieve[4]; // the 5th is the msb and the 4th is the lsb of the payload Length
  int sizeToRecieve = payLoadLength + CHECKSUM_SIZE;

  unique_ptr<uint8[]> recievePayLoad( new uint8[sizeToRecieve] );
  LC_CHECKRETURNLOGERROR( 
	( SOCKET_ERROR == m_tcpClient->Receive( recievePayLoad.get(), sizeToRecieve ) ),
	"Packet receiving error" ); 

  return true;
}

LightCrafter::LCR_Command::LCR_Command( uint8 packetType, uint16 commandId, uint8 flags ) :
m_payloadLength( 0 )
{
  m_payLoad = unique_ptr<uint8[]>( new uint8[MAX_PACKET_SIZE] );
  // Initialize the command header
  m_payLoad[0] = packetType;
  m_payLoad[1] = (commandId >> 8) & 0xFF;
  m_payLoad[2] = commandId & 0xFF;
  m_payLoad[3] = flags;
}

unsigned long LightCrafter::LCR_Command::AppendPayload( uint8* payload, unsigned long payLoadLength )
{
  unsigned long appended = m_payloadLength + payLoadLength < MAX_PAYLOAD_SIZE ?
	payLoadLength : MAX_PAYLOAD_SIZE - m_payloadLength;

  // Insert the payload after any existing payload
  for( unsigned int i = 0; i < appended; i++)
	{ m_payLoad[HEADER_SIZE + m_payloadLength + i] = payload[i]; }
  m_payloadLength += appended;

  return appended;
}

void LightCrafter::LCR_Command::SetPacketType( uint8 packetType )
  { m_payLoad[0] = packetType; }

unsigned long LightCrafter::LCR_Command::GetCommandSize( void )
{ return HEADER_SIZE + m_payloadLength + CHECKSUM_SIZE; }

uint8* LightCrafter::LCR_Command::GetCommand( void )
{
  // Set the correct length and calculate the checksum
  m_payLoad[4] = m_payloadLength & 0xFF;
  m_payLoad[5] = ( m_payloadLength >> 8 ) & 0xFF;

  // Append the checksum
  int sum = 0;
  for( unsigned long i = 0; i < m_payloadLength; ++i )
	{ sum += m_payLoad[i]; }
  m_payLoad[HEADER_SIZE + m_payloadLength] = (uint8)(sum & 0xFF);

  return m_payLoad.get( );
}