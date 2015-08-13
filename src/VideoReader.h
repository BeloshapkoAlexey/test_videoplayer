#pragma once

#include <opencv2/imgproc/imgproc.hpp>

#include "VideoFrame.h"


class VideoReader{
	/* implements reading video from an abstract source*/
public:
	virtual IplImage* GetNextFramePointer() = 0;	//returns internal next frame pointer

	virtual double GetFrameRate() = 0;				//video frame rate
	virtual double GetTimePositionMs() = 0;			//time position of the current frame 

	virtual int GetFrameDepth() = 0;				//video depth
	virtual int GetFrameNumOfChannels() = 0;		//video channels

	virtual CvSize* GetFrameSize() = 0;				//video size
	virtual VideoFrameProperties* GetFrameProps() = 0; //video frame properties
}; 