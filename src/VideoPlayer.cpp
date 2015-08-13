#include "VideoPlayer.h"
#include "FileVideoReader.h"

#include "moc_VideoPlayer.moc"

VideoPlayer::VideoPlayer(int& argc, char** argv)
{
	m_imgHandler = new ImageHandler();
	m_gui = new VideoGui(m_imgHandler, argc, argv);

	void(VideoGui::*loadFilesSender)(char*, char*) =	//connects loading video files
		&VideoGui::LoadTheseFiles;						//from gui to the player
	void(VideoPlayer::*loadFilesReceiver)(char*, char*) = &VideoPlayer::InitializeVideo;
	QObject::connect(m_gui, loadFilesSender, this, loadFilesReceiver);
}

VideoPlayer::~VideoPlayer()
{
	delete m_imgHandler;
	delete m_gui;

	delete m_reader1;
	delete m_reader2;

	delete m_buffer;

	delete m_readerC1;
	delete m_readerC2;

	delete m_handlerC;
}

void VideoPlayer::Run()
{
	m_gui->ShowLoadFiles();	//tells gui to ask a user for 
}							//video files

void VideoPlayer::InitializeVideo(char* videoPath1, char* videoPath2)
{
	m_reader1 = new FileVideoReader(videoPath1);	//file readers
	m_reader2 = new FileVideoReader(videoPath2);

	m_buffer = new MultiThreadVideoBuffer(15);	//main buffer

	m_readerC1 = new ReaderToBufferController(	//reading thread workers
		m_reader1, m_buffer, 1);
	m_readerC2 = new ReaderToBufferController(
		m_reader2, m_buffer, 2);
	m_handlerC = new BufferToImageHandlerController(	//from-buffer worker
		m_buffer, m_imgHandler);

	ReadingThread* readingThread1 = new ReadingThread(m_readerC1);	//file reading threads
	ReadingThread* readingThread2 = new ReadingThread(m_readerC2);

	VizualizingThread* vizualizingThread =		//from buffer to gui thread
		new VizualizingThread(m_handlerC);
	m_handlerC->moveToThread(vizualizingThread);	//to get signals from gui 

	void(BufferToImageHandlerController::*framesPreparedSender)(void) =
		&BufferToImageHandlerController::FramesPrepared;	//connecting from-buffer 
															//thread to gui
	void(VideoGui::*framesPreparedReceiver)(void) = &VideoGui::UpdateVideoImages;
	QObject::connect(m_handlerC, framesPreparedSender, m_gui, framesPreparedReceiver);

	void(VideoGui::*updateCompletedSender)(void) = &VideoGui::UpdateCompleted;
	void(BufferToImageHandlerController::*updateCompletedReceiver)(void) =
		&BufferToImageHandlerController::PrepareFrames;		//connecting gui to thread
	QObject::connect(m_gui, updateCompletedSender, m_handlerC, updateCompletedReceiver);

	readingThread1->start();	//starting threads
	readingThread2->start();

	vizualizingThread->moveToThread(vizualizingThread);
	vizualizingThread->start();

	readingThread1->start();
	readingThread2->start();
	vizualizingThread->start();

	emit InitializationDone();
}

InitializationThread::InitializationThread()
{

}

void InitializationThread::run()
{
	this->exec();	//accepts video player initialization signal
}