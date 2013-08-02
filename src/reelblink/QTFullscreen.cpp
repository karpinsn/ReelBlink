#include "QTFullscreen.h"
#include "windows.h"


QTFullscreen::QTFullscreen(int screenNumber ) : screenNumber(screenNumber)
{
	screenres = QApplication::desktop()->screenGeometry(screenNumber);
	picture.reset(new QLabel());
}


bool QTFullscreen::ProjectImage( cv::Mat image )
{
	if (image.rows < 1 || image.cols < 1){
		throw std::logic_error("Image size too small.");
	}

	//Ensure we either have a 1-channel or 3-channel input image
	QImage::Format mFormat;
	if (image.channels() == 3) mFormat = QImage::Format_RGB888;
	else if (image.channels() == 1) mFormat = QImage::Format_Indexed8;
	else throw std::logic_error("ProjectImage takes either a mono or 3-channel image.");
	
	//Note: mImage will default to white per QT spec if the input image is invalid
	auto mImage = QImage((uchar*) image.data, image.cols, image.rows, image.step, mFormat);
	picture->setPixmap(QPixmap::fromImage(mImage));
	
	//Move to proper location and show fullscreen
	picture->move(screenres.x(), screenres.y());
	picture->showFullScreen();

	return true;
}

int QTFullscreen::GetWidth()
{
	return screenres.width();
}


int QTFullscreen::GetHeight()
{
	return screenres.height();
}

