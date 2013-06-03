#ifndef _LIGHTCRAFTER_H_
#define _LIGHTCRAFTER_H_

using namespace std;

#include <iostream>
#include <string>

#include "LCR_Commander.h"
#include "LCR_Common.h"
#include "../IProjector.h"
#include "BitmapCreator.h"



#define LCR_Default_IP			"192.168.1.100" 
#define LCR_Default_PORT		"21845"


class LightCrafter//: public IProjector
{
private:
	bool IsConnected;
	LCR_Commander*  Commander;
	unique_ptr<uint8[]> _Convert2BinaryImage(cv::Mat byteImage);

	static const uint8 MSB_HIGH  = 0x80;
    static const uint8 MSB_LOW	  = 0x7F;


public:
	LightCrafter(void);
	~LightCrafter(void);

	void Connect();
	void Disconnect();
	bool LightCrafter::StaticDisplayMode(DisplayMode displayMode);
	bool ProjectImage(cv::Mat image);

	int LightCrafter::GetHeight(void);

	int LightCrafter::GetWidth(void);
};

#endif