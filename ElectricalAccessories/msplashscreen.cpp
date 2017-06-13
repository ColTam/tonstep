#include "msplashscreen.h"

#include <QProgressBar>
#include <QLabel>
#include <QDebug>

mSplashScreen::mSplashScreen(const QPixmap &pixmap) : QSplashScreen(pixmap)
{
    mProgressBar = new QProgressBar(this);
    mProgressBar->setGeometry(1,this->height()-17,this->width()-2,17);
    mProgressBar->setRange(0, 100);
    mProgressBar->setValue(0);

    mProgressBar->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    mProgressBar->setTextVisible(false);

    mProgressBar->setStyleSheet("QProgressBar{"
                                "border: none; "
                                "border-radius: 3px; "
                                "text-align: center; "
                                "background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #383838, stop:1 #282828);}"
                                "QProgressBar::chunk {"
                                "background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #D7801A, stop: 1 #F46320);"
                                "border-radius: 3px; margin: 0.5px;}");

    connect(mProgressBar, &QProgressBar::valueChanged, [this]() {repaint();});//更新进度条显示

    mLabel = new QLabel(this);
    mLabel->setGeometry(1,this->height()-16,this->width()-2,15);
    mLabel->setStyleSheet("color:#F0F0F0;");
    mLabel->setAlignment(Qt::AlignCenter);
}

mSplashScreen::~mSplashScreen()
{
}

void mSplashScreen::setProgress(int value)
{
    mProgressBar->setValue(value);
}

void mSplashScreen::setLabelTest(QString str)
{
    mLabel->setText(str);
}
