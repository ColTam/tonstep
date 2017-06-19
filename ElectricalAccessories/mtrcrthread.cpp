#include "mtrcrthread.h"
#include "Collect.h"
#include "formdisplay.h"

#include <QMutex>
#include <QTimer>
#include <QThread>
#include <QDebug>
#include <QEvent>

mTRCRThread::mTRCRThread(QObject *parent) : QObject(parent)
  ,oldTc("")
{
}

mTRCRThread::~mTRCRThread()
{
    delete _trTimer;
}

bool mTRCRThread::mtRCRIsActive()
{
    return _trTimer->isActive();
}

void mTRCRThread::readData()
{
    _trTimer = new QTimer;
    connect(_trTimer, SIGNAL(timeout()), this, SLOT(updateTRCR()), Qt::DirectConnection);
}

int i = 0;
void mTRCRThread::updateTRCR()
{
    QMutex mutex;
    mutex.lock();
//    QThread::sleep(1);
    QString str;
    QString tc;

    readList.clear();
    FormDisplay f;
    if (f.mTRType == FormDisplay::YOKOGAWA_GP10) {
        str = "FData,0,0001,0110";
        tc = CollectControl::HardSend(devInformation.at(1).com, str.toLatin1().data(), COM_DELAY);
        qDebug() << tc.size();
        if (tc.size() < 723 && !oldTc.isEmpty()) {
            QThread::msleep(1000);
            qDebug() << "GGGGGGGGGGGGGGGGGGGG@@@@@@@@@@@@@ tc receive failed @@@@@@@@@@@@@GGGGGGGGGGGGGGGGGG";
            tc = oldTc;
        }
        oldTc = tc;

        readList << tc.split("TT");
        Collect::getTemperatureRise(&readList);
    }
    emit tRCRData(readList);
    mutex.unlock();
}

void mTRCRThread::mTRCRTimerStart()
{
    _trTimer->start(COM_TRDELAY);
}

void mTRCRThread::mTRCRTimerStop()
{
    _trTimer->stop();
}

