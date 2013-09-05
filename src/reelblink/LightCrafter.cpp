#include "LightCrafter.h"

LightCrafter::LightCrafter( )
{
  m_tcpClient = unique_ptr<Tcp>( new Tcp( ) );
  
  Connect();
  PatternDisplayMode( );
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

  // Set for pattern display with just a single pattern that we will change with ProjectImage
  LCR_Command displayModeCommand( Host_Write, CurrentDisplayMode, DataComplete );
  uint8 payload = (uint8)PatternSequenceDisplay; // set the display mode
  displayModeCommand.AppendPayload( &payload, 1 );
  LC_CHECKRETURNLOGERROR( SendLCRCommand( displayModeCommand ), "LCR Set Display mode send has send error" );

  // Stop the pattern projection so we can adjust the settings
  LCR_Command stopCommand( Host_Write, StatPatternSequence, DataComplete );
  uint8 stop = 0; stopCommand.AppendPayload( &stop, 1 );
  LC_CHECKRETURNLOGERROR( SendLCRCommand( stopCommand ), "Unable to stop pattern projection" );

  // Modify the pattern settings
  LCR_Command patternSeqCommand( Host_Write, PatternSequencing, DataComplete );
  uint8 bitDepth = 1; patternSeqCommand.AppendPayload( &bitDepth, 1 );
  uint8 patterns = 1; patternSeqCommand.AppendPayload( &patterns, 1 );
  uint8 invert   = 0; patternSeqCommand.AppendPayload( &invert,   1 );
  uint8 trigger  = 1; patternSeqCommand.AppendPayload( &trigger,  1 );
  uint32_t delay   = 0; patternSeqCommand.AppendPayload( (uint8*)(&delay), 4 );
  uint32_t period  = 33333; patternSeqCommand.AppendPayload( (uint8*)(&period), 4 );
  uint32_t exposure = 33333; patternSeqCommand.AppendPayload( (uint8*)(&exposure), 4 );
  // On newer LightCrafter this means all LEDs. On the older one it will default to green
  uint8 led      = 3; patternSeqCommand.AppendPayload( &led, 1 );

  // http://e2e.ti.com/support/dlp__mems_micro-electro-mechanical_systems/f/850/t/219181.aspx
  uint8 blah     = 0; patternSeqCommand.AppendPayload( &blah, 1 );
  LC_CHECKRETURNLOGERROR( SendLCRCommand( patternSeqCommand ), "LCR Pattern Seq error" );

  // Start pattern projection
  LCR_Command startCommand( Host_Write, StatPatternSequence, DataComplete );
  uint8 start = 1; startCommand.AppendPayload( &start, 1 );
  LC_CHECKRETURNLOGERROR( SendLCRCommand( startCommand ), "Unable to start pattern projection" );

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

  // Stop the pattern projection so we can adjust the settings
  LCR_Command stopCommand( Host_Write, StatPatternSequence, DataComplete );
  uint8 stop = 0; stopCommand.AppendPayload( &stop, 1 );
  LC_CHECKRETURNLOGERROR( SendLCRCommand( stopCommand ), "Unable to stop pattern projection" );

  // Step 2 - Packatize and stream
  unsigned long offset = 0;
  unsigned long byteCount = imageStream->step;

  // First packet has the patternNo at the beginning
  LCR_Command firstPacket( Host_Write, PatternDefinition, Beginning );
  uint8 patternNo = 0; firstPacket.AppendPayload( &patternNo, 1 );
  offset += firstPacket.AppendPayload( imageStream->data.ptr + offset, byteCount - offset );
  LC_CHECKRETURNLOGERROR( SendLCRCommand( firstPacket ), "Unable to issue command to light crafter" );

  // The remaining is just the rest of the image
  while( offset < byteCount )
  {
	LCR_Command command( Host_Write, PatternDefinition, Intermediate );
	offset += command.AppendPayload( imageStream->data.ptr + offset, byteCount - offset );
	if( offset >= byteCount ) // Check to see if we hit the end, if so signify in the flags
	  { command.SetFlags( End ); }

	LC_CHECKRETURNLOGERROR( SendLCRCommand( command ), "Unable to issue command to light crafter" );
  }
  
  // Start pattern projection
  LCR_Command startCommand( Host_Write, StatPatternSequence, DataComplete );
  uint8 start = 1; startCommand.AppendPayload( &start, 1 );
  LC_CHECKRETURNLOGERROR( SendLCRCommand( startCommand ), "Unable to start pattern projection" );

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
	( SOCKET_ERROR != m_tcpClient->Receive( recievePayLoad.get(), sizeToRecieve ) ),
	"Packet receiving error" ); 

  return LCR_Write_Response == recieve[0] || LCR_ReponsePacket == recieve[0];
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
  memcpy( &(m_payLoad[HEADER_SIZE + m_payloadLength] ), payload, appended );
  m_payloadLength += appended;

  return appended;
}

void LightCrafter::LCR_Command::SetFlags( uint8 flags )
  { m_payLoad[3] = flags; }

unsigned long LightCrafter::LCR_Command::GetCommandSize( void )
{ return HEADER_SIZE + m_payloadLength + CHECKSUM_SIZE; }

uint8* LightCrafter::LCR_Command::GetCommand( void )
{
  // Set the correct length and calculate the checksum
  m_payLoad[4] = m_payloadLength & 0xFF;
  m_payLoad[5] = ( m_payloadLength >> 8 ) & 0xFF;

  // Append the checksum
  uint8 sum = 0;
  for( unsigned long i = 0; i < HEADER_SIZE + m_payloadLength; ++i )
	{ sum += m_payLoad[i]; }
  m_payLoad[HEADER_SIZE + m_payloadLength] = (uint8)(sum & 0xFF);

  return m_payLoad.get( );
}