#pragma once

#include <QtCore\qelapsedtimer.h>
#include <QtCore\qthread.h>

#include "VideoBuffer.h"
#include "VideoReader.h"
#include "ImageHandler.h"
#include "ImageConverter.h"

class ReaderToBufferController
{
	/*Do work to retreive video frames from file and
	push it to the buffer*/
public:
	ReaderToBufferController(VideoReader* reader, MultiThreadVideoBuffer* buffer, int index);
	~ReaderToBufferController();

	void MoveVideoFrame();

private:
	VideoReader* m_reader;
	MultiThreadVideoBuffer* m_buffer;
	IplImage* m_frame;

	int m_index;
};

class BufferToImageHandlerController : public QObject
{
	/*Do the work of converting and copying most relevant frames from 
	buffer to image handler.*/
	Q_OBJECT
public:
	BufferToImageHandlerController(MultiThreadVideoBuffer* buffer,
		ImageHandler* imageHandler);	
	~BufferToImageHandlerController();

	void StartTimer();	//starts internal video timer

public slots:
	void PrepareFrames();	//gotten a signal from gui
							//starts copying frames
signals:
	void FramesPrepared();	//sent to gui when frames are ready

private:
	MultiThreadVideoBuffer* m_buffer;	//main buffer
	ImageHandler* m_imageHandler;		//image handler

	QVector<int> m_threadIndexes;		//mapping between thread indexes
										//and internal indexes
	QVector<VideoFrame*>* m_frames;		//allocated space for copying frames
	QVector<VideoFrame*>* m_retrievedFrames;	//pointers that are sent to
										//the buffer for reading
	QVector<double> m_frameDurations;	//inverted frames rates

	QElapsedTimer m_timer;				//internal video timer
	ImageConverter m_imgConverter;		//converter from IplImage to QImage
};

class ReadingThread : public QThread
{
public:
	ReadingThread(ReaderToBufferController* controller);
	~ReadingThread();

	void run();

private:
	ReaderToBufferController* m_controller;
};

class VizualizingThread : public QThread
{

public:
	VizualizingThread(BufferToImageHandlerController* controller);
	~VizualizingThread();

	void run();

private:
	BufferToImageHandlerController* m_controller;
};