#include "BitmapCreator.h"


CvMat* BitmapCreator::CreateBitmapFromMat(cv::Mat image)
{
  const int para[]  = {CV_IMWRITE_PXM_BINARY, 1, 0}; 
  CvMat cvMat = image;
  return cvEncodeImage(".bmp", &cvMat, para);
};
