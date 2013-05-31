#ifndef PROJECTOR_LIGHT_COMMANDER_H_
#define PROJECTOR_LIGHT_COMMANDER_H_

#include <QObject>

#include <algorithm>
#include <memory>
#include <PortabilityLayer.h>

#include "IProjector.h"

using namespace std;

class LightCommanderProjector : public IProjector
{
  Q_OBJECT

private:
  static const Byte MSB_HIGH  = 0x80;
  static const Byte MSB_LOW	  = 0x7F;

public:
  LightCommanderProjector(void);
  ~LightCommanderProjector(void);
  bool Init( void );
  bool ProjectImage(cv::Mat image);
  int GetWidth( void );
  int GetHeight( void );
  bool SetLEDIntensity(float r, float g, float b, float ir);

private:
  bool _SetLED(LED_t led, float intensity);
  unique_ptr<Byte[]> _Convert2BinaryImage(cv::Mat byteImage);
  
 /**
  * Used to write registers to start the light commander projecting.
  * This is undocumented and taken from sniffing their communication,
  * so it is subject to change. Using this you dont have to run their software
  */
  bool _WritePlayRegister( void );
  bool _CheckLogError(Status status);
};

#define DLP_CHECKRETURNBATCHCOMMAND(COMMAND) \
{ \
  if( !_CheckLogError( RunBatchCommand(COMMAND) ) )\
	{ return false; } \
}

void LightCommanderCCallback(String loggingCallback);

#endif //PROJECTOR_LIGHT_COMMANDER_H_
