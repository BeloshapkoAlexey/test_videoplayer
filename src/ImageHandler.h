#pragma once

#include <QtGui/qimage.h>
#include <QtCore/qmutex.h>

class ImageHandler
{
	/*Some kind of buffer to hold images before visualization*/
public:
	ImageHandler();
	~ImageHandler();

	void AddImage(int width, int height, int index);//adds place for new image
	QImage* GetImage(int index);					//returns a pointer to image
	double GetTimestamp(int index);					//returns associated timestamp

	void SetTimestamp(double value, int index);		//updates associated timestamp

private:
	QVector<QImage*> m_images;						//image pointers
	QVector<double> m_timestamps;					//timestamps

	QVector<int> m_indexes;							//mapping between thread indexes
};													//and internal vector indexes