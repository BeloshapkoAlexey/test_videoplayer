#pragma once

#include <QtWidgets\qwidget.h>
#include <QtWidgets\qlabel.h>
#include <QtGui\qpainter.h>
#include <QtWidgets\qboxlayout.h>

#include "ImageHandler.h"

class DoubleVideoWidget : public QWidget
	/*GUI element witch displays two images*/
{
public:
	DoubleVideoWidget(int sizeRatio, QWidget* pwgt = 0);//bigger ratio - 
														//smaller second video.	
	void RedrawVideoFrames(QImage* biggerFrame,			//draws given frames
		QImage* smallerFrame);

protected:
	void paintEvent(QPaintEvent* event);

private:
	int m_sizeRatio;					//minimum ratio of sides of two frames

	QImage* m_biggerFrame;				//pointers for paintEvent
	QImage* m_smallerFrame;				//contains images to draw				
};

class VideoGui : public QObject
{
	/*Gui with two video displays and two time labels*/
	Q_OBJECT
public:
	VideoGui(ImageHandler* imageHandler,
		int& argc, char** argv);
	~VideoGui();

	void ShowSelf();							//should show self
	void ShowLoadFiles();						//now opens two dialog windows
												//for opening videos
public slots:
	void UpdateVideoImages();					//causes redrawing videos

signals:
	void LoadTheseFiles(char* video1,			//user opened videofiles paths
		char* video2);
	void UpdateCompleted();						//tells system to load next 
												//frames to image handler
protected:
	void setTimes(double biggetTime,			//sets time labels to given 
		double smallerTime);					//timestamps in millis.
	void convertTime(double timeMs,				//used to convert millis into
		QString& result);						//time label's text.

private:
	QApplication* m_app;
	QWidget* m_mainWidget;						//the window

	DoubleVideoWidget* m_videoWidget;			//main video widget

	QLabel* m_timeLabel1;						//time labels
	QLabel* m_timeLabel2;
	QString m_timeString1;						//used in time labels
	QString m_timeString2;

	ImageHandler* m_imgHandler;					//container with frames to redraw
	bool m_isInitialized;						//used to set window 
												//size correctly
};