#include "mupdatethread.h"
#include "Collect.h"

mUpdateThread::mUpdateThread(QObject *parent) : QObject(parent)
{

}

void mUpdateThread::updateState()
{
    QString volt = CollectControl::HardSend(comVolt, QString("RDW VF").toLatin1().data(), 1000);
    devInformation.removeAt(0);
    UART_t voltUart;
    voltUart.data = volt;
    voltUart.com = comVolt;
    devInformation.insert(0,voltUart);

    QString tc = CollectControl::HardSend(comTR_YOKOGAWAGP10, QString("FData,0,0001,0110").toLatin1().data(), 1000);
    devInformation.removeAt(1);
    UART_t trUart;
    trUart.data = tc;
    trUart.com = comTR_YOKOGAWAGP10;
    devInformation.insert(1,trUart);

    QString loadA = CollectControl::HardSend(comLoadA, QString("RDM%101%102%\r").toLatin1().data(), 1000);
    devInformation.removeAt(2);
    UART_t loadAUart;
    loadAUart.data = loadA;
    loadAUart.com = comLoadA;
    devInformation.insert(2,loadAUart);

    QString loadB = CollectControl::HardSend(comLoadB, QString("RDM%101%102%\r").toLatin1().data(), 1000);
    devInformation.removeAt(3);
    UART_t loadBUart;
    loadBUart.data = loadB;
    loadBUart.com = comLoadB;
    devInformation.insert(3,loadBUart);

    QString loadC = CollectControl::HardSend(comLoadC, QString("RDM%101%102%\r").toLatin1().data(), 1000);
    devInformation.removeAt(4);
    UART_t loadCUart;
    loadCUart.data = loadC;
    loadCUart.com = comLoadC;
    devInformation.insert(4,loadCUart);

    QString testA = CollectControl::HardSend(comTest, QString("RDWD020603").toLatin1().data(), 1000);
    devInformation.removeAt(5);
    UART_t testAUart;
    testAUart.data = testA;
    testAUart.com = comTest;
    devInformation.insert(5,testAUart);

    QString testB = CollectControl::HardSend(comTest, QString("RDWD030603").toLatin1().data(), 1000);
    devInformation.removeAt(6);
    UART_t testBUart;
    testBUart.data = testB;
    testBUart.com = comTest;
    devInformation.insert(6,testBUart);

    QString testC = CollectControl::HardSend(comTest, QString("RDWD040603").toLatin1().data(), 1000);
    devInformation.removeAt(7);
    UART_t testCUart;
    testCUart.data = testC;
    testCUart.com = comTest;
    devInformation.insert(7,testCUart);

    QString tc2 = CollectControl::HardSend(comTR_AGILENT34970, QString("*IDN?\r").toLatin1().data(), 1000);
    devInformation.removeAt(8);
    UART_t trUart2;
    trUart2.data = tc2;
    trUart2.com = comTR_AGILENT34970;
    devInformation.insert(8,trUart2);

    emit updatedState();
}
