#ifndef PROJECTOR_I_PROJECTOR_H_
#define PROJECTOR_I_PROJECTOR_H_

#include <cv.h>

class IProjector
{
public:
	virtual bool ProjectImage(cv::Mat image) = 0;
	virtual int GetWidth() = 0;
	virtual int GetHeight() = 0;
};

#endif //PROJECTOR_I_PROJECTOR_H_