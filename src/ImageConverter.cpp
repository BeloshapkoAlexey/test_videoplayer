#include"ImageConverter.h"


ImageConverter::ImageConverter()
{

}

ImageConverter::~ImageConverter()
{

}

void ImageConverter::IplImageToQImage(IplImage* iplFrom, QImage* qTo)
{
	if (!iplFrom->imageData)	//if no source image
		return;
	
	if (qTo->width() != iplFrom->width)	//if width doesn't match
	{
		throw new VideoException("IplImageToQImage: widths do not match");
		return;
	}

	if (qTo->height() < iplFrom->height)	//if not enougth height
	{
		throw new VideoException("IplImageToQImage: heights do not match");
		return;
	}

	IplImage* iplImg = iplFrom;
	QImage* qimg = qTo;

	int h = iplImg->height;
	int w = iplImg->width;
	int channels = iplImg->nChannels;

	char *data = iplImg->imageData;

	if (channels == 1)	//gray image
	{
		for (int y = 0; y < h; y++, data += iplImg->widthStep)
		{
			for (int x = 0; x < w; x++)
			{
				memset(qimg->scanLine(y) + sizeof(int)*x, data[x], sizeof(char));
			}
		}
	}
	else if (channels == 3)	//normal case
	{
		for (int y = 0; y < h; y++, data += iplImg->widthStep)
		{
			for (int x = 0; x < w; x++)
			{
				memcpy(qimg->scanLine(y) + sizeof(int)* x,
					data + channels * x, channels * sizeof(char));
			}
		}
	}
	else if (channels == 4)	//normal case
	{
		for (int y = 0; y < h; y++, data += iplImg->widthStep)
		{
			memcpy(qimg->bits() + y * qimg->bytesPerLine(),
				data, iplImg->widthStep);	//string-wise copying
		}
	}	
}