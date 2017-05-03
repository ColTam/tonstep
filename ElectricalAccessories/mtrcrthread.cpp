#include "mtrcrthread.h"
#include "Collect.h"

#include <QMutex>
#include <QTimer>
#include <QThread>
#include <QDebug>
#include <QEvent>

mTRCRThread::mTRCRThread(QObject *parent) : QObject(parent)
{
    oldTc = "";
//    "EATTDATE 17/03/10TTTIME 10:53:45.300 TTN 0001    掳C       +00000219E-01TT\
//N 0002    掳C       +00000221E-01TTN 0003    掳C       +00000222E-01TT\
//N 0004    掳C       +00000221E-01TTN 0005    掳C       +00000220E-01TT\
//N 0006    掳C       +00000221E-01TTN 0007    掳C       +00000221E-01TT\
//N 0008    掳C       +00000221E-01TTN 0009    掳C       +00000221E-01TT\
//N 0010    掳C       +00000221E-01TTN 0101    掳C       +00000221E-01TT\
//N 0102    掳C       -00000221E-01TTN 0103    掳C       +00000221E-01TT\
//N 0104    掳C       +00000222E-01TTN 0105    掳C       +00000221E-01TT\
//N 0106    掳C       +00000221E-01TTN 0107    掳C       +00000220E-01TT\
//N 0108    掳C       +00000221E-01TTN 0109    掳C       +00000220E-01TT\
//N 0110    掳C       +00000221E-01TTEN";
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
    connect(_trTimer, SIGNAL(timeout()), this, SLOT(updaeTRCR()));
}

int i = 0;
void mTRCRThread::updaeTRCR()
{
    QMutex mutex;
    mutex.lock();
//    QThread::sleep(1);
    QString str;
    QString tc;

    readList.clear();
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
    emit tRCRData(readList);
    mutex.unlock();
}

void mTRCRThread::mTRCRTimerStart()
{
    _trTimer->start(5000);
}

void mTRCRThread::mTRCRTimerStop()
{
    _trTimer->stop();
}

