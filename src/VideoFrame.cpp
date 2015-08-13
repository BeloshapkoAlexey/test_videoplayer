#include "VideoFrame.h"

VideoFrame::VideoFrame(CvSize* frameSize, int frameDepth, int frameChannels, double timestamp)
{
	m_frame = cvCreateImage(*frameSize, frameDepth, frameChannels);
	m_timestamp = timestamp;
}

VideoFrame::VideoFrame(VideoFrameProperties* props, double timestamp) 
{
	m_frame = cvCreateImage(*props->GetSize(), props->GetDepth(),
		props->GetNChannels());
	m_timestamp = timestamp;
}

VideoFrame::~VideoFrame()
{
	cvReleaseImage(&m_frame);
}

IplImage* VideoFrame::GetFrame()
{
	return m_frame;
}

double VideoFrame::GetTimestamp()
{
	return m_timestamp;
}

void VideoFrame::SetFrame(IplImage* frame)
{
	cvCopy(frame, m_frame, NULL);
}

void VideoFrame::SetTimestamp(double timestamp)
{
	m_timestamp = timestamp;
}

void VideoFrame::CopyTo(VideoFrame* frame)
{
	cvCopy(m_frame, frame->GetFrame(), NULL);
	frame->SetTimestamp(m_timestamp);
}

VideoFrameProperties::VideoFrameProperties(CvSize* size, int depth, int nChannels, double framerate)
{
	m_size = new CvSize(*size);
	m_depth = depth;
	m_nChannels = nChannels;
	m_frameRate = framerate;
}

VideoFrameProperties::~VideoFrameProperties()
{
	delete m_size;
}

CvSize* VideoFrameProperties::GetSize()
{
	return m_size;
}

int VideoFrameProperties::GetDepth()
{
	return m_depth;
}
int VideoFrameProperties::GetNChannels()
{
	return m_nChannels;
}

double VideoFrameProperties::GetFrameRate()
{
	return m_frameRate;
}