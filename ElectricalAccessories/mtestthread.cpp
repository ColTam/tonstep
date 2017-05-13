#include "mtestthread.h"
#include "Collect.h"

#include <QTimer>
#include <QDebug>
#include <QMutex>
#include <QEvent>

mTestThread::mTestThread(QObject *parent) : QObject(parent)
    , oldNumberA("000000000064\r\n")
    , oldNumberB("000000000064\r\n")
    , oldNumberC("000000000064\r\n")
{
}

mTestThread::~mTestThread()
{
    delete _testTimer;
}

void mTestThread::readData()
{
    _testTimer = new QTimer;
    connect(_testTimer, SIGNAL(timeout()), this, SLOT(updaeTest()), Qt::DirectConnection);

//    mTestTimerStart();
}

void mTestThread::updaeTest()
{
    QMutex mutex;
    mutex.lock();
    QString number;

    if (flag == 0) {
        number  = CollectControl::HardSend(devInformation.at(5).com, QString("RDWD020603").toLatin1().data(), COM_DELAY);
        if (number.size() != 13 || number.isEmpty()) {
            number = oldNumberA;
        }
        oldNumberA = number;

        flag+=1;
        emit testDataA(number);
    } else if (flag == 1) {
        number  = CollectControl::HardSend(devInformation.at(6).com, QString("RDWD030603").toLatin1().data(), COM_DELAY);
        if (number.size() != 13 || number.isEmpty()) {
            number = oldNumberB;
        }
        oldNumberB = number;

        flag+=1;
        emit testDataB(number);
    } else if (flag == 2) {
        number = CollectControl::HardSend(devInformation.at(7).com, QString("RDWD040603").toLatin1().data(), COM_DELAY);
        if (number.size() != 13 || number.isEmpty()) {
            number = oldNumberC;
        }
        oldNumberC = number;

        flag = 0;
        emit testDataC(number);
    }
    mutex.unlock();
}

void mTestThread::mTestTimerStart()
{
    _testTimer->start(250);//4-250/3-
}

void mTestThread::mTestTimerStop()
{
    _testTimer->stop();
}

bool mTestThread::mTestIsActive()
{
    return _testTimer->isActive();
}
