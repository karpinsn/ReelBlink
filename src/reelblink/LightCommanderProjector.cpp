#include "LightCommanderProjector.h"


LightCommanderProjector::LightCommanderProjector(void)
{

}


LightCommanderProjector::~LightCommanderProjector(void)
{
}

bool LightCommanderProjector::projectImage(cv::Mat image)
{
  return false;
}

int LightCommanderProjector::getWidth()
{
  // According to the documentation the 
  // LightCommander will only ever project this size
  return 1024;
}

int LightCommanderProjector::getHeight()
{
  // According to the documentation the 
  // LightCommander will only ever project this size
  return 768;
}