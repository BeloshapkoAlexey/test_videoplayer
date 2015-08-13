#pragma once

#include "VideoReader.h"

#include <opencv2/highgui/highgui.hpp>

class FileVideoReader : public VideoReader
{
	/* implements reading video from file*/
public:
	FileVideoReader(char* filename);		//opens a video file given the path
	~FileVideoReader();

	IplImage* GetNextFramePointer();	//returns internal next frame pointer
	
	double GetFrameRate();				//video frame rate
	double GetTimePositionMs();			//time position of the current frame 

	int GetFrameDepth();				//video depth
	int GetFrameNumOfChannels();		//video channels

	CvSize* GetFrameSize();				//video size
	VideoFrameProperties* GetFrameProps(); //video frame properties
private:
	void readNextFrame();				//grabs new frame

	CvCapture* m_capture;				//video source
	IplImage* m_frame;					//pointer to the frame
	CvSize* m_frameSize;				//video size

	VideoFrameProperties* m_frameProperties;

	bool m_isInitialized;				//used to initialize size of
};										//frames