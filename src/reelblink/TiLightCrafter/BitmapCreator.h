#ifndef _BITMAP_CREATOR_H_
#define _BITMAP_CREATOR_H_

#include <cv.h>
#include <opencv2/highgui/highgui.hpp>
#include "LCR_Common.h"

#define BITMAP_SIZE = 1247670;



class BitmapCreator 
{
public:
   static CvMat* CreateBitmapFromMat(cv::Mat image);
};




#endif