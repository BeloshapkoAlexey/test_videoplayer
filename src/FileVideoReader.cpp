#include "FileVideoReader.h"

FileVideoReader::FileVideoReader(char* filename)
{
	m_isInitialized = false;

	m_capture = cvCreateFileCapture(filename);
	readNextFrame();		//reads first frame: we need this
							//to get right video parameters
	if (m_frame != 0)
	{
		m_frameSize = new CvSize();	//setting size of the video
		m_frameSize->height = m_frame->height;
		m_frameSize->width = m_frame->width;

		m_frameProperties = new VideoFrameProperties(m_frameSize,
			GetFrameDepth(), GetFrameNumOfChannels(), GetFrameRate());
	}

	else m_frameSize = 0;	//error
}

IplImage* FileVideoReader::GetNextFramePointer()
{
	if (!m_isInitialized)
		m_isInitialized = true;
	else
		readNextFrame();

	return m_frame;	
}

double FileVideoReader::GetFrameRate()
{
	return cvGetCaptureProperty(m_capture, CV_CAP_PROP_FPS);
}

double FileVideoReader::GetTimePositionMs()
{
	return cvGetCaptureProperty(m_capture, CV_CAP_PROP_POS_MSEC);
}

int FileVideoReader::GetFrameDepth()
{
	if (m_frame != 0) return m_frame->depth;
	else return 0;
}

int FileVideoReader::GetFrameNumOfChannels()
{
	if (m_frame != 0) return m_frame->nChannels;
	else return 0;
}

CvSize* FileVideoReader::GetFrameSize()
{
	return m_frameSize;
}

VideoFrameProperties* FileVideoReader::GetFrameProps()
{
	return m_frameProperties;
}

FileVideoReader::~FileVideoReader()
{
	cvReleaseCapture(&m_capture);
	delete m_frameSize;

	delete m_frameProperties;
}

void FileVideoReader::readNextFrame()
{
	if ((m_capture == 0) || (!cvGrabFrame(m_capture))) m_frame = 0;	//checks if the source is 
																		//valid and tries to grab the next frame
	else m_frame = cvRetrieveFrame(m_capture);		//if successful - reads the next frame
}