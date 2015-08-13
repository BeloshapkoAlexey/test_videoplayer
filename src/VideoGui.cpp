#include <QtWidgets\qapplication.h>
#include <QtWidgets\qfiledialog.h>

#include "VideoGui.h"

#include "moc_VideoGui.moc"

DoubleVideoWidget::DoubleVideoWidget(int sizeRatio, QWidget* pwgt) : QWidget(pwgt)
{
	m_sizeRatio = sizeRatio;

	m_smallerFrame = 0;
	m_biggerFrame = 0;
}

void DoubleVideoWidget::RedrawVideoFrames(QImage* biggerFrame, QImage* smallerFrame)
{
	m_biggerFrame = biggerFrame;
	m_smallerFrame = smallerFrame;

	this->repaint();
}

void DoubleVideoWidget::paintEvent(QPaintEvent* event)
{
	
	
	QPainter* painter = new QPainter(this);

	if ((m_biggerFrame == 0) || (m_smallerFrame == 0)) 
		return;

	int smallerWidth, smallerHeight;
	if (m_sizeRatio == 0)
	{
		smallerWidth = m_smallerFrame->width();
		smallerHeight = m_smallerFrame->height();
	}
	else
	{
		double scale;

		double widthScale = ( (double) m_smallerFrame->width()) / m_biggerFrame->width();
		double heightScale = ( (double) m_smallerFrame->height()) / m_biggerFrame->height();

		if (widthScale > heightScale)
			scale = 1.0 / widthScale / m_sizeRatio;
		else
			scale = 1.0 / heightScale / m_sizeRatio;

		smallerWidth = (int) (scale * m_smallerFrame->width());
		smallerHeight = (int)(scale * m_smallerFrame->height());
	}
	
	//painter->setRenderHint(QPainter::Antialiasing);

	painter->setViewport(0, 0, width(), height());
	painter->setWindow(0, 0, m_biggerFrame->width(), m_biggerFrame->height());

	painter->drawImage(0, 0, *m_biggerFrame);

	painter->setViewport(width() - smallerWidth, height() - smallerHeight,
		smallerWidth, smallerHeight);
	painter->setWindow(0, 0, m_smallerFrame->width(), m_smallerFrame->height());

	painter->drawImage(0, 0, *m_smallerFrame);
}

VideoGui::VideoGui(ImageHandler* imageHandler, int& argc, char** argv)
{
	m_imgHandler = imageHandler;

	m_app = new QApplication(argc, argv);

	m_mainWidget = new QWidget();

	m_videoWidget = new DoubleVideoWidget(2);
	m_timeLabel1 = new QLabel("0:00");
	m_timeLabel2 = new QLabel("0:00");

	QBoxLayout* timeLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	QBoxLayout* mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);

	timeLayout->addWidget(m_timeLabel1);
	timeLayout->addWidget(m_timeLabel2);

	mainLayout->addWidget(m_videoWidget, 10);
	mainLayout->addLayout(timeLayout, 1);

	m_mainWidget->setLayout(mainLayout);

	m_mainWidget->resize(600, 400);
	m_isInitialized = false;
}

VideoGui::~VideoGui()
{

}

void VideoGui::UpdateVideoImages()
{
	double time1 = m_imgHandler->GetTimestamp(1);
	double time2 = m_imgHandler->GetTimestamp(2);

	this->setTimes(time1, time2);

	QImage* biggerImage = m_imgHandler->GetImage(1);
	QImage* smallerImage = m_imgHandler->GetImage(2);

	if ((biggerImage == 0) || (smallerImage == 0)) return;

	if (!m_isInitialized)
	{
		if (biggerImage->width() > 640)
			m_mainWidget->setFixedSize(biggerImage->width(), biggerImage->height() + 50);
		else
		{
			int height = 640 * biggerImage->height() / biggerImage->width();
			m_mainWidget->setFixedSize(640, height + 50);
			m_videoWidget->setFixedSize(640, height);
		}
	}

	double scale = ((double)biggerImage->width()) / m_videoWidget->width();
	int newHeight = (int) (biggerImage->height() / scale);

	int main_width = m_mainWidget->width();
	m_videoWidget->setFixedSize(m_videoWidget->width(), newHeight);


	m_videoWidget->RedrawVideoFrames(m_imgHandler->GetImage(1), m_imgHandler->GetImage(2));



	emit UpdateCompleted();
}

void VideoGui::setTimes(double biggerTime, double smallerTime)
{
	convertTime(biggerTime, m_timeString1);
	m_timeLabel1->setText(m_timeString1);

	convertTime(smallerTime, m_timeString2);
	m_timeLabel2->setText(m_timeString2);
}

void VideoGui::convertTime(double timeMs, QString& result)
{
	long millis = (long)timeMs;

	long seconds = millis / 1000;
	millis = millis % 1000;

	long minutes = seconds / 60;
	seconds = seconds % 60;

	long hours = minutes / 60;
	minutes = minutes % 60;

	result = QString::number(hours) + ':' + QString::number(minutes) + ':' +
		QString::number(seconds) + ':' + QString::number(millis);
}

void VideoGui::ShowLoadFiles()
{
	QFileDialog dialog;
	QByteArray arr1 = dialog.getOpenFileName().toLocal8Bit();
	char* file1 = arr1.data();
	QByteArray arr2 = dialog.getOpenFileName().toLocal8Bit();
	char* file2 = arr2.data();

	emit LoadTheseFiles(file1, file2);
	this->ShowSelf();
}

void VideoGui::ShowSelf()
{
	m_mainWidget->show();
	m_app->exec();
}