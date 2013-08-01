#include "QTFullscreen.h"


bool QTFullscreen::ProjectImage( cv::Mat image )
{
	if (image.rows < 1 || image.cols < 1){
		throw std::logic_error("Image size too small.");
	}

	QLabel *picture = new QLabel();
	auto mImage = QImage((uchar*) image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
	picture->setPixmap(QPixmap::fromImage(mImage));
	picture->showFullScreen();
}

int QTFullscreen::GetWidth()
{
	throw std::logic_error("The method or operation is not implemented.");
}

int QTFullscreen::GetHeight()
{
	throw std::logic_error("The method or operation is not implemented.");
}

