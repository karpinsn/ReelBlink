#ifndef PROJECTOR_I_PROJECTOR_H_
#define PROJECTOR_I_PROJECTOR_H_


class IProjector
{
public:
	virtual bool projectImage(cv::Mat image) = 0;
	virtual int getWidth() = 0;
	virtual int getHeight() = 0;
};

#endif //PROJECTOR_I_PROJECTOR_H_