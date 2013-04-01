#ifndef PROJECTOR_LIGHT_COMMANDER_H_
#define PROJECTOR_LIGHT_COMMANDER_H_

#define MSB_HIGH 0x10000000
#define MSB_LOW 0x01111111

#include <memory>
#include <PortabilityLayer.h>

#include "IProjector.h"

using namespace std;

class LightCommanderProjector : public IProjector
{
public:
  LightCommanderProjector(void);
  ~LightCommanderProjector(void);
  bool Init( void );
  bool ProjectImage(cv::Mat image);
  int GetWidth( void );
  int GetHeight( void );
private:
  unique_ptr<Byte[]> _Convert2BinaryImage(cv::Mat byteImage);
  bool _CheckLogError(Status status);
};

void LightCommanderCCallback(String loggingCallback);

#endif //PROJECTOR_LIGHT_COMMANDER_H_
