#include <limits.h>

#include "VideoThreads.h"

#include "moc_VideoThreads.moc"


ReaderToBufferController::ReaderToBufferController(VideoReader* reader,
	MultiThreadVideoBuffer* buffer, int index)
{
	m_buffer = buffer;
	m_reader = reader;
	m_index = index;

	m_frame = 0;

	m_buffer->AddVideoThread(m_index, m_reader->GetFrameProps());
}

ReaderToBufferController::~ReaderToBufferController()
{

}

void ReaderToBufferController::MoveVideoFrame()
{
	m_frame = m_reader->GetNextFramePointer();
	double timestamp = m_reader->GetTimePositionMs();

	m_buffer->WriteVideoFrame(m_frame, timestamp, m_index);
}

BufferToImageHandlerController::BufferToImageHandlerController(
	MultiThreadVideoBuffer* buffer, ImageHandler* imageHandler)
{
	m_buffer = buffer;
	m_imageHandler = imageHandler;

	m_frames = new QVector<VideoFrame*>();
	m_retrievedFrames = new QVector<VideoFrame*>();

	QList<int>* indexes = m_buffer->GetVideoThreadsIndexes();	//getting thread indexes from buffer

	QList<int>::iterator indexes_iter;
	for (indexes_iter = indexes->begin(); indexes_iter != indexes->end(); indexes_iter++)
	{
		m_threadIndexes.push_back(*indexes_iter);	//adding thread index into account

		VideoFrameProperties* frameProps = m_buffer->GetFrameProps(*indexes_iter);	//creating internal frames
		VideoFrame* frame = new VideoFrame(frameProps);

		m_frames->push_back(frame);	//pushing internal frames
		m_retrievedFrames->push_back(frame);

		m_frameDurations.push_back(1000.0 / frameProps->GetFrameRate());	//rememberring frameRates

		m_imageHandler->AddImage(frameProps->GetSize()->width,	//adding images to imageHandler
			frameProps->GetSize()->height, *indexes_iter);
	}
}

BufferToImageHandlerController::~BufferToImageHandlerController()
{
	QVector<int>::iterator iter;
	for (iter = m_threadIndexes.begin(); iter != m_threadIndexes.end(); iter++)
	{
		cvDestroyWindow("video " + *iter);
		delete (*m_frames)[*iter];
	}

}

void BufferToImageHandlerController::PrepareFrames()
{
	long copytime = m_timer.elapsed();
	m_buffer->ReadVideoFrames(m_retrievedFrames, &m_threadIndexes, m_timer.elapsed());

	//std::cout << m_timer.elapsed() - copytime << std::endl;

	long minTimeShown = std::numeric_limits<long>::max();	//should be shown next

	for (int i = 0; i < m_threadIndexes.size(); i++)
	{
		if ((*m_retrievedFrames)[i] != 0)
		{
			VideoFrame* frame = (*m_frames)[i];

			m_imgConverter.IplImageToQImage(frame->GetFrame(), m_imageHandler->GetImage(m_threadIndexes[i]));
			m_imageHandler->SetTimestamp(frame->GetTimestamp(), m_threadIndexes[i]);	//copying to gui imgHandler
		}

		double time = (*m_frames)[i]->GetTimestamp() + m_frameDurations[i];

		if (time < minTimeShown)
			minTimeShown = (long)time;

		(*m_retrievedFrames)[i] = (*m_frames)[i];
	}

	long sleepTime = minTimeShown - m_timer.elapsed();
	if (sleepTime > 0)
	{
		unsigned long casted_sleepTime = (unsigned long)sleepTime;
		//std::cout << "sleeping: " << casted_sleepTime << std::endl;
		QThread::currentThread()->msleep(casted_sleepTime);
	}


	emit FramesPrepared();
}

void BufferToImageHandlerController::StartTimer()
{
	m_timer.start();
}

ReadingThread::ReadingThread(ReaderToBufferController* controller)
{
	m_controller = controller;
}

ReadingThread::~ReadingThread()
{
	delete m_controller;
}

void ReadingThread::run()
{
	while (true)
	{
		m_controller->MoveVideoFrame();
	}
}

VizualizingThread::VizualizingThread(BufferToImageHandlerController* controller)
{
	m_controller = controller;
}

VizualizingThread::~VizualizingThread()
{
	
}

void VizualizingThread::run()
{
	m_controller->StartTimer();

	m_controller->PrepareFrames();
	
	this->exec();
}