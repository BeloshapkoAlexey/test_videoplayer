#include "ImageHandler.h"

ImageHandler::ImageHandler() : 
m_indexes(),
m_timestamps()
{
	
}

ImageHandler::~ImageHandler()
{
	for (int i = 0; i < m_images.size(); i++)
	{
		if (m_images[i] != 0)
			delete m_images[i];
	}
}

QImage* ImageHandler::GetImage(int index)
{
	int internal_index = m_indexes.indexOf(index);

	if (internal_index != -1)
		return m_images[internal_index];
	else return 0;
}

double ImageHandler::GetTimestamp(int index)
{
	int internal_index = m_indexes.indexOf(index);

	if (internal_index != -1)
		return m_timestamps[internal_index];
	else return -2.0;
}

void ImageHandler::SetTimestamp(double value, int index)
{
	int internal_index = m_indexes.indexOf(index);

	if (internal_index != -1)
		m_timestamps[internal_index] = value;
}

void ImageHandler::AddImage(int width, int height, int index)
{
	int internal_index = m_indexes.indexOf(index);
	if (internal_index != -1)	
	{
		delete m_images[internal_index];
		m_images[internal_index] = new QImage(width, height, QImage::Format_RGB32);
		return;
	}
	else
	{
		m_indexes.push_back(index);
		m_timestamps.push_back(-1.0);
		m_images.push_back(new QImage(width, height, QImage::Format_RGB32));
	}
	
}
