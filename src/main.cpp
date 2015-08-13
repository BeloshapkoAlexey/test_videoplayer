#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>

#include "VideoPlayer.h"

int main(int argc, char* argv[])
{
	VideoPlayer* player = new VideoPlayer(argc, argv);
	InitializationThread* initThread = new InitializationThread();

	player->moveToThread(initThread);	//to receive initVideo signal from load gui
	initThread->start();
	player->Run();	//should show load gui

	return 0;
}