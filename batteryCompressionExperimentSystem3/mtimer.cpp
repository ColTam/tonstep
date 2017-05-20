#include "mtimer.h"
#include "widget.h"

#include <QTimer>
#include <QDebug>

mTimer::mTimer(QObject *parent) : QObject(parent)
{

}

mTimer::~mTimer()
{

}

//void mTimer::mTimerStart()
//{
//    m_timer->start(200);
//}

//void mTimer::mTimerStop()
//{
//    m_timer->stop();
//}

void mTimer::initThread()
{
    m_timer = new QTimer();
//    connect(m_timer, &QTimer::timeout, this, &mTimer::mTimerout);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(mTimerout()));
    m_timer->start(200);
}

void mTimer::mTimerout()
{
    if (Widget::m_serialPort != nullptr) {
        const unsigned char rd[8] = {0x0A, 0x03, 0x00, 0x00, 0x00, 0x0A, 0xC4, 0xB6};
        Widget::m_serialPort->write((char *)rd, sizeof rd);
    } else {
        qDebug() << "222";
    }
}
