#include "VideoBuffer.h"


#include <iostream>

SingleThreadVideoFramePool::SingleThreadVideoFramePool(int poolSize, CvSize* frameSize, int frameDepth, int frameChannels)
{
	m_poolSize = poolSize;

	m_freeVideoFrames = new bool[m_poolSize];

	for (int i = 0; i < m_poolSize; i++)
		m_freeVideoFrames[i] = false;

	m_memory = new VideoFrame*[m_poolSize];
	for (int index = 0; index < m_poolSize; index++)
	{
		m_memory[index] = new VideoFrame(frameSize, frameDepth, frameChannels);
	}

	m_lastGivenCell = -1;
	m_lastFreedCell = 0;
	m_numOfGivenOut = 0;
}

VideoFrame* SingleThreadVideoFramePool::GetFreeVideoFrame()
{
	if (m_numOfGivenOut == m_poolSize) return 0;	//if full

	do {
		m_lastGivenCell++;							//go through the array to
		m_lastGivenCell %= m_poolSize;				//find a free cell
	} while (m_freeVideoFrames[m_lastGivenCell]);	

	m_numOfGivenOut++;
	m_freeVideoFrames[m_lastGivenCell] = true;		//mark as busy

	return m_memory[m_lastGivenCell];
}

void SingleThreadVideoFramePool::ReleaseVideoFrame(VideoFrame* frame)
{
	int index = -1;
	
	for (int i = m_lastFreedCell + 1; i != m_lastFreedCell;
		i++, i %= m_poolSize)										//compute the index of 
	{																		//a cell being released
		if (m_memory[i] == frame)
		{
			index = i;
			break;
		}
	}

	if (m_memory[m_lastFreedCell] == frame)
	{
		index = m_lastFreedCell;
	}

	if (index == -1)
		return;

	m_freeVideoFrames[index] = false;	//mark as free
	m_lastFreedCell = index;
	m_numOfGivenOut--;
}

SingleThreadVideoFramePool::~SingleThreadVideoFramePool()
{
	delete[] m_freeVideoFrames;

	for (int index = 0; index < m_poolSize; index++)
	{
		delete m_memory[index];
	}
	delete[] m_memory;
}

SingleThreadVideoBuffer::SingleThreadVideoBuffer(int size, CvSize* frameSize, int frameDepth, int frameChannels) :
	m_framesPool(size, frameSize, frameDepth, frameChannels)
{
	m_size = size;
}

SingleThreadVideoBuffer::~SingleThreadVideoBuffer() {}

bool SingleThreadVideoBuffer::WriteVideoFrame(IplImage* frame, double timestamp)
{
	VideoFrame* freeFrame = m_framesPool.GetFreeVideoFrame();
	if (freeFrame == 0) return false;

	freeFrame->SetFrame(frame);
	freeFrame->SetTimestamp(timestamp);
	m_queue.append(freeFrame);

	return true;
}

bool SingleThreadVideoBuffer::ReadVideoFrame(VideoFrame* frame, double timestamp)
{
	if ((m_queue.isEmpty()) || (m_queue.head()->GetTimestamp() > timestamp))
	{
		return false;	//if there is no relevant frames
	}
	int size = this->Size();
	int unrelevant = 0;

	VideoFrame* result = m_queue.dequeue();
	while (!m_queue.isEmpty())	//go through a queue and find the first
	{							//frame with latter timestamp
		if (m_queue.head()->GetTimestamp() < timestamp)
		{
			m_framesPool.ReleaseVideoFrame(result);
			result = m_queue.dequeue();	//the previous to the first latter frame
										//will be copied, others deleted
			unrelevant++;
		}
		else break;
	}
	
	result->CopyTo(frame);	
	m_framesPool.ReleaseVideoFrame(result);

	return true;
}

int SingleThreadVideoBuffer::Size()
{
	return m_queue.size();
}

MultiThreadVideoBuffer::MultiThreadVideoBuffer(int size) :
m_access(),
m_waitForReadMutex(),
m_WCMutex(),
m_readCond(),
m_readCondMutex()
{
	m_size = size;
	m_maxPoolIndex = -1;

	m_writeCount = 0;
	m_waitForReadCount = 0;
}

MultiThreadVideoBuffer::~MultiThreadVideoBuffer()
{
	QList<int>::iterator iter;
	for (iter = m_threadsIndexes.begin(); iter != m_threadsIndexes.end(); iter++)
	{
		delete m_buffers[*iter];
		delete m_props[*iter];
	}
}

void MultiThreadVideoBuffer::AddVideoThread(int index, CvSize* frameSize, 
	int frameDepth, int frameChannels, double frameRate)
{
	m_threadsIndexes.push_back(index);

	if (m_maxPoolIndex < index)
	{
		m_maxPoolIndex = index;
		m_buffers.resize(m_maxPoolIndex + 1);
		m_props.resize(m_maxPoolIndex + 1);
	}

	m_buffers[index] = new SingleThreadVideoBuffer(m_size,
		frameSize, frameDepth, frameChannels);
	m_props[index] = new VideoFrameProperties(frameSize, frameDepth, frameChannels, frameRate);
}

void MultiThreadVideoBuffer::AddVideoThread(int index,
	VideoFrameProperties* props)
{
	AddVideoThread(index, props->GetSize(), props->GetDepth(),
		props->GetNChannels(), props->GetFrameRate());
}

VideoFrameProperties* MultiThreadVideoBuffer::GetFrameProps(int index)
{
	if (m_threadsIndexes.contains(index))
		return m_props[index];
	else return 0;
}

QList<int>* MultiThreadVideoBuffer::GetVideoThreadsIndexes()
{
	return &m_threadsIndexes;
}

void MultiThreadVideoBuffer::WriteVideoFrame(IplImage* frame, double timestamp, int videoThread)
{

	if (m_waitForReadCount != 0)//if reading is awaits, let it be
	{
		m_readCondMutex.lock();
		m_readCond.wait(&m_readCondMutex);
		m_readCondMutex.unlock();
	}

	enterWriting();

	bool success = false;
	while (!success)
	{
		success = writeVideoFrame(frame, timestamp, videoThread);	//try to write
		if (!success)
		{
			leaveWriting();
			
			m_readCondMutex.lock();
			m_readCond.wait(&m_readCondMutex);	//if the buffer is full, wait for reading
			m_readCondMutex.unlock();

			enterWriting();
		}
	}

	leaveWriting();
}

void MultiThreadVideoBuffer::ReadVideoFrames(QVector<VideoFrame*>* videoFrames, QVector<int>* threadIndexes, double timestamp)
{
	m_access.lock();	

	m_waitForReadMutex.lock();
	m_waitForReadCount++;		//activate reading indicator
	m_waitForReadMutex.unlock();

	readVideoFrames(videoFrames, threadIndexes, timestamp);	//read

	m_waitForReadMutex.lock();
	m_waitForReadCount--;		//deactivate reading indicator
	m_waitForReadMutex.unlock();

	m_readCond.wakeAll();		//wake up writing threads that waited for reading

	m_access.unlock();
}

void MultiThreadVideoBuffer::enterWriting()
{
	m_WCMutex.lock();

	if (m_writeCount == 0)
	{
		m_access.lock();	//take the access rights if no writers was
	}
				
	m_writeCount++;				//increment writing threads number

	m_WCMutex.unlock();
}

void MultiThreadVideoBuffer::leaveWriting()
{

	m_WCMutex.lock();
	m_writeCount--;		//decrement writers count
	if (m_writeCount == 0)
	{
		m_access.unlock();	//leaving access rightes
	}

	m_WCMutex.unlock();
}

bool MultiThreadVideoBuffer::writeVideoFrame(IplImage* frame, double timestamp, int videoThread)
{
	bool bufferIsFull;

	if (m_threadsIndexes.contains(videoThread)) 
		bufferIsFull = !m_buffers[videoThread]->WriteVideoFrame(frame, timestamp);
	else
	{
		throw new VideoException(
			"writing to uncreated video thread in MultiThreadVideoBuffer::writeVideoFrame\n");
		return true;
	}

	return !bufferIsFull;
}

void MultiThreadVideoBuffer::readVideoFrames(QVector<VideoFrame*>* videoFrames, 
	QVector<int>* threadIndexes, double timestamp)
{
	for (int i = 0; i < threadIndexes->size(); i++)
	{
		bool wasRead = false;
		int index = (*threadIndexes)[i];
		if (m_threadsIndexes.contains(index))
			wasRead = m_buffers[index]->ReadVideoFrame((*videoFrames)[i], timestamp);

		if (!wasRead)	//buffer is empty or data is too new or index is inactive
			(*videoFrames)[i] = 0;
	}
}