#include "msplashscreen.h"

#include <QProgressBar>
#include <QLabel>

mSplashScreen::mSplashScreen(const QPixmap &pixmap) : QSplashScreen(pixmap)
{
    mProgressBar = new QProgressBar(this);
    mProgressBar->setGeometry(1,this->height()-23,this->width()-2,23);
    mProgressBar->setRange(0, 100);
    mProgressBar->setValue(0);

    mProgressBar->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    mProgressBar->setTextVisible(false);

    mProgressBar->setStyleSheet("QProgressBar{border: 2px solid rgb(216,216,218); border-radius: 4px; text-align:center; background:rgb(50, 50, 50);}"
                                "QProgressBar::chunk {background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffa02f, stop: 1 #d7801a); border-radius: 3px;}");

    connect(mProgressBar, &QProgressBar::valueChanged, [this]() {repaint();});//更新进度条显示

    mLabel = new QLabel(this);
    mLabel->setGeometry(1,this->height()-23,this->width()-2,23);
    mLabel->setStyleSheet("color:rgb(235,235,235)");
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
