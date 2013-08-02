#include "IProjector.h"
#include <QLabel>
#include <QObject>
#include <qdesktopwidget>
#include <QApplication>
#include <memory>

class QTFullscreen : public IProjector

{
	Q_OBJECT
public:
	QTFullscreen(int screenNumber = 0);
	~QTFullscreen(void){};

	bool ProjectImage( cv::Mat image );

	int GetWidth();

	int GetHeight();

private:
	int screenNumber;
	QRect screenres;
	std::unique_ptr<QLabel> picture;

};