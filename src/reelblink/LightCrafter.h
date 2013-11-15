#ifndef _LIGHTCRAFTER_H_
#define _LIGHTCRAFTER_H_

using namespace std;

#include <iostream>
#include <string>
#include <WinSock2.h>
#include <memory>

#include <cv.h>
#include <highgui.h>

#include "Tcp.h"
#include "LCR_Common.h"
#include <stdint.h>

#include "IProjector.h"

#define LCR_Default_IP			"192.168.1.100" 
#define LCR_Default_PORT		"21845"
#define ConnectionAttempts      5

class LightCrafter : public IProjector
{
private:
  unique_ptr<Tcp> m_tcpClient;

  class LCR_Command
  {
  private:
	unsigned long m_payloadLength;
	unique_ptr<uint8[]> m_payLoad;

  public:
	LCR_Command( uint8 packetType, uint16 commandId, uint8 flags );
	unsigned long AppendPayload( uint8* payload, unsigned long payLoadLength = MAX_PAYLOAD_SIZE );

	void SetFlags( uint8 flags );
	unsigned long GetCommandSize( void );
	uint8* GetCommand( void );
  };

public:
	LightCrafter();
	
	bool Connect();
	bool Disconnect();

	// Resolution of the projector is set to 608 x 684 (Size of DMD)
	int GetWidth(void)  { return 608; }
	int GetHeight(void) { return 684; }

	bool CacheImages( vector<cv::Mat> images );
	bool ProjectCachedImage( uint8 imageNumber );
	bool ProjectImage( cv::Mat image );

private:
  bool SendImage( uint8 imageNumber, cv::Mat image );
  bool PatternDisplayMode( uint8 patternCount = 1 );
  bool SendLCRCommand( LCR_Command& command );
};

#define LC_CHECKRETURNLOGERROR( COMMAND, ERROR ) \
{ \
  if( !COMMAND ) \
	{ cout << ERROR << endl; return false; } \
}

#endif