#pragma once
#include <opencv2/imgproc/imgproc.hpp>

class VideoFrameProperties
{
	/*Holds video frame properties*/
public:
	VideoFrameProperties(CvSize* size, int depth, int nChannels,
		double frameRate);
	~VideoFrameProperties();

	CvSize* GetSize();
	int GetDepth();
	int GetNChannels();
	double GetFrameRate();

private:
	CvSize* m_size;
	int m_depth;
	int m_nChannels;

	double m_frameRate;
};

class VideoFrame
{
	/*Data structure to contain information about video frames.*/
public:
	VideoFrame(CvSize* frameSize, int frameDepth, int frameChannels,
		double timestamp = -1.0);	//creates a frame with given parameters
	VideoFrame(VideoFrameProperties* props,
		double timestamp = -1.0);

	~VideoFrame();
	IplImage* GetFrame();			//returns iternal pointer to the frame
	double GetTimestamp();			//video timestamp

	void SetFrame(IplImage* frame);	//copies a given frame image
	void SetTimestamp(double timestamp);	//sets timestamp

	void CopyTo(VideoFrame* frame);	//copies a given frame
private:
	IplImage* m_frame;				//frame image
	double m_timestamp;				//video frame timestamp

	bool m_isFrameExternal;
};

