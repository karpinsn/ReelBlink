#ifndef PROJECTOR_LIGHT_COMMANDER_H_
#define PROJECTOR_LIGHT_COMMANDER_H_

#include "IProjector.h"

class LightCommanderProjector : public IProjector
{
public:
	LightCommanderProjector(void);
	~LightCommanderProjector(void);
	bool projectImage(cv::Mat image);
	int getWidth();
	int getHeight();
};

#endif //PROJECTOR_LIGHT_COMMANDER_H_
