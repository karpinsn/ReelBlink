#include "IProjector.h"
#include <QLabel>
#include <QObject>


class QTFullscreen : public IProjector

{
	Q_OBJECT
public:
	QTFullscreen(void){};
	~QTFullscreen(void){};

	bool ProjectImage( cv::Mat image );

	int GetWidth();

	int GetHeight();

private:
	int width, height;

};