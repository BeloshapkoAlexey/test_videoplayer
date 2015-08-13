#pragma once

#include <opencv2/imgproc/imgproc.hpp>
#include <QtGui/qimage.h>

#include "VideoException.h"

class ImageConverter
{
	/*tool class that can convert from iplImage to QImage*/
public:
	ImageConverter();
	~ImageConverter();
	void IplImageToQImage(IplImage* iplFrom, QImage* qTo);
};