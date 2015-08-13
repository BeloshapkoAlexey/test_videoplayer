#pragma once

#include <QtCore/qobject.h>

#include "VideoReader.h"
#include "VideoBuffer.h"
#include "VideoThreads.h"
#include "VideoGui.h"

class VideoPlayer : public QObject
{
	/*Main class: creates all the components and wires them*/
	Q_OBJECT
public:
	VideoPlayer(int& argc, char** argv);	//creates gui	
	~VideoPlayer();

	void Run();								//makes gui to ask for video files

public slots :
	void InitializeVideo(char* videoPath1,	//given video files paths, creates
		char* videoPath2);					//other components and runs threads

signals:
	void InitializationDone();				//signal sent after InitializeVideo
											//now unused
private:
	ImageHandler* m_imgHandler;				//image container to redraw
	VideoGui* m_gui;						//main gui

	VideoReader* m_reader1;					//reads first file
	VideoReader* m_reader2;					//reads second file

	MultiThreadVideoBuffer* m_buffer;		//main synchronized buffer

	ReaderToBufferController* m_readerC1;	//do work in threads that read files
	ReaderToBufferController* m_readerC2;	

	BufferToImageHandlerController* m_handlerC;	//do work in thread that 
};												//takes from the buffer

class InitializationThread : public QThread
	/*thread that runs to initialize VideoPlayer,
	because main thread is used by gui*/
{
public:
	InitializationThread();
	void run();
};