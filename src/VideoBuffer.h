#pragma once
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <QtCore\qqueue.h>
#include <QtCore\qvector.h>
#include <QtCore\qmutex.h>
#include <QtCore\qwaitcondition.h>

#include "VideoFrame.h"
#include "VideoException.h"

class SingleThreadVideoFramePool
{
	/*Intent: to allocate and manage memory for video frames.
	Works at constant time if the user holds the principle of
	"first taken - first released"*/
public:
	SingleThreadVideoFramePool(int poolSize, CvSize* frameSize,		
		int frameDepth, int frameChannels);		//allocates memory for poolSize
												//frames with given parameters
	~SingleThreadVideoFramePool();

	VideoFrame* GetFreeVideoFrame();			//returns pointer to a free frame
	void ReleaseVideoFrame(VideoFrame* frame);	//marks a given frame as free

private:
	VideoFrame** m_memory;						//frames pointers array

	bool* m_freeVideoFrames;					//flags to indicate if a pointer
												//with given index is free
	int m_poolSize;								//size
	int m_numOfGivenOut;						//number of given out pointers
												//to control the pool's emptiness.

	int m_lastGivenCell;						//index of last given out frame
												//makes giving th frames out 
												//mostly circular.
	int m_lastFreedCell;						//index of last freed frame
};

class SingleThreadVideoBuffer
{
	/*Buffered queue of video frames*/
public:
	SingleThreadVideoBuffer(int bufferSize, CvSize* frameSize, 
		int frameDepth, int frameChannels);	//allocates memory for bufferSize frames
	~SingleThreadVideoBuffer();

	bool WriteVideoFrame(IplImage* frame,
		double timestamp);					//copies the given frame image
											// returns true if is not full
	bool ReadVideoFrame(VideoFrame* frame,	//copies the most relevant video 
		double timestamp);					//frame to a given timestamp or return false.
											//older ones are removed from buffer								
	int Size();								//number of frames in queue

private:
	int m_size;								//max size
	QQueue<VideoFrame*> m_queue;			//queue
	SingleThreadVideoFramePool m_framesPool;//memory pool
};

class MultiThreadVideoBuffer
	/*Synchronized buffer to contain video frames 
	from different video threads.*/
{
public:
	MultiThreadVideoBuffer(int size);	//creates buffer for maximum "size" video
										//frames for each thread
	~MultiThreadVideoBuffer();

	void AddVideoThread(int index, CvSize* frameSize,	//adds new thread of video 
		int frameDepth, int frameChannels,				//frames to the buffer and
		double frameRate);								//associates it to a given index.
														
	void AddVideoThread(int index,
		VideoFrameProperties* props);

	VideoFrameProperties* GetFrameProps(int index);		//returns pointer to video size

	QList<int>* GetVideoThreadsIndexes();				//returns active thread
														//indexes.

	void WriteVideoFrame(IplImage* frame,				//copies video frame image and time
		double timestamp, int videoThread);				//to a buffer with given thread index

	void ReadVideoFrames(QVector<VideoFrame*>* videoFrames,	//reads most relevant video
		QVector<int>* threadIndexes, double timestamp);		//frames. If no such, writes
															//null at corresponding index.
protected:
	bool writeVideoFrame(IplImage* frame,	//unsynchronized operations of read and write
		double timestamp, int videoThread);	
	void readVideoFrames(QVector<VideoFrame*>* videoFrames, QVector<int>* threadIndexes, double timestamp);

	void enterWriting();
	void leaveWriting();
private:
	QVector<SingleThreadVideoBuffer*> m_buffers;	//buffers
	QVector<VideoFrameProperties*> m_props;			//properties of frames
	QList<int> m_threadsIndexes;					//active thread indexes

	int m_size;										//max buffer length for each thread
	int m_maxPoolIndex;								//size of indexes vector

	int m_writeCount;								//number of threads reading
	QMutex m_WCMutex;								//mutex on that number

	QMutex m_access;								//read-write mutex

	int m_waitForReadCount;							//indicates that read thread awaits
	QMutex m_waitForReadMutex;						//mutex on that integer
	QWaitCondition m_readCond;						//condition for writing threads to 
													//sleep until the next read occurs
	QMutex m_readCondMutex;							//mutex for that
};