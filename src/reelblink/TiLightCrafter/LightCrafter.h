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

class LightCrafter : public IProjector
{
private:
	bool IsConnected;
	unique_ptr<LCR_Commander> Commander;

public:
	LightCrafter();
	
	bool Connect();
	bool Disconnect();

	int GetHeight(void);
	int GetWidth(void);
	
	bool StaticDisplayMode();
	bool ProjectImage(cv::Mat image);
};

#endif