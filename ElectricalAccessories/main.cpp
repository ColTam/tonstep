/***************************************************************************
**                                                                        **
**  email: jason0x4@outlook.com                                           **
**  address: https://github.com/jason0x4/tonstep.git                      **
**  单样品、完整流程运行成功                                              **
**  同类型多样品流程成功运行                                              **
**  不同类型多样品测试待优化                                              **
**                                                                        **
****************************************************************************/

/* 2017-05-02 18:01:07 +0200 */

#include "widget.h"
#include "msplashscreen.h"
#include "Collect.h"

#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include <QDesktopWidget>
#include <Windows.h>
#include <QSettings>
#include <QDebug>

#pragma execution_character_set("utf-8")

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));

    Widget w;
//    QCoreApplication coreApp(argc, argv);
//    FormDisplay::dirName = a.applicationDirPath();
    FormDisplay::dirName = QCoreApplication::applicationDirPath();
    //read setting .ini
    mSplashScreen *splash = new mSplashScreen(QPixmap(":/image/image/SGS01.jpg"));
    QSettings *configIniRead = new QSettings("EATconfig.ini", QSettings::IniFormat);

    QString voltageU    = configIniRead->value("Voltage Uart").toString();
    QString TRU1        = configIniRead->value("Temperature Rise Uart1").toString();
    QString TRU2        = configIniRead->value("Temperature Rise Uart2").toString();
    QString LoadAU      = configIniRead->value("Load(ServoA) Uart").toString();
    QString LoadBU      = configIniRead->value("Load(ServoB) Uart").toString();
    QString LoadCU      = configIniRead->value("Load(ServoC) Uart").toString();
    QString lifeTestU   = configIniRead->value("Life Tester Uart").toString();
    QString path        = configIniRead->value("Default Path").toString();
    int language        = configIniRead->value("Language").toInt();

    //load language  cn/en
    QTranslator translator;
    translator.load( ":/other/zh_CN.qm" );
    if (language == 1) {
        a.installTranslator( &translator );
    }

    splash->show();
    splash->setGeometry((QApplication::desktop()->width()-480)/2,(QApplication::desktop()->height()-270)/2,480,270);
    splash->setLabelTest(QObject::tr("Is connecting STAS and Configure the local file"));
    splash->setProgress(1);     //

    if (!voltageU.isEmpty()){
        comVolt  = voltageU.toInt();
    }
    if (!TRU1.isEmpty()){
        comTR_YOKOGAWAGP10 = TRU1.toInt();
    }
    if (!TRU2.isEmpty()){
        comTR_AGILENT34970 = TRU2.toInt();
    }
    if (!LoadAU.isEmpty()){
        comLoadA = LoadAU.toInt();
    }
    if (!LoadBU.isEmpty()){
        comLoadB = LoadBU.toInt();
    }
    if (!LoadCU.isEmpty()){
        comLoadC = LoadCU.toInt();
    }
    if (!lifeTestU.isEmpty()){
        comTest  = lifeTestU.toInt();
    }
    if (!path.isEmpty()) {
        FormDisplay::dirName = path;
    }
    delete configIniRead;

    Sleep(2000);
    //voltage               0
    splash->setLabelTest(QObject::tr("Checking the Power Supply UART..."));
    splash->setProgress(100/10+(qrand()%10-5));

    CollectControl::HardSend(comVolt, QString("RDW VF").toLatin1().data(),200);
    QString volt = CollectControl::HardSend(comVolt, QString("RDW VF").toLatin1().data(),900);
    UART_t voltUart;
    if (!volt.isEmpty() && volt.size() == 14) {
        Sleep(700);
        voltUart.data = volt;
    } else {
        voltUart.data = "";
    }
    voltUart.com = comVolt;
    devInformation << voltUart;
    //temperature rise      1       yokogawa Gp10
    splash->setLabelTest(QObject::tr("Checking the Yokogawa Paperless Recorder UART...."));
    splash->setProgress(200/10+(qrand()%10-5));

    QString trcr = CollectControl::HardSend(comTR_YOKOGAWAGP10, QString("FData,0,0001,0110").toLatin1().data(),1100);
    UART_t trcrUart;
    if (!trcr.isEmpty() && trcr.size() == 723) {
        Sleep(700);
        trcrUart.data = trcr;
    } else {
        trcrUart.data = "";
    }
    trcrUart.com = comTR_YOKOGAWAGP10;
    devInformation << trcrUart;
    //loadA                 2
    splash->setLabelTest(QObject::tr("Checking the Load(ServoA) UART..."));
    splash->setProgress(300/10+(qrand()%10-5));

    QString loadA = CollectControl::HardSend(comLoadA, QString("RDM%101%102%\r").toLatin1().data(),1100);
    UART_t loadAUart;
    if (!loadA.isEmpty() && loadA.size() > 15) {//load.size() == [16, 19]
        Sleep(700);
        loadAUart.data = loadA;
    } else {
        loadAUart.data = "";
    }
    loadAUart.com = comLoadA;
    devInformation << loadAUart;
    //loadB                 3
    splash->setLabelTest(QObject::tr("Checking the Load(ServoB) UART...."));
    splash->setProgress(400/10+(qrand()%10-5));

    QString loadB = CollectControl::HardSend(comLoadB, QString("RDM%101%102%\r").toLatin1().data(),1100);
    UART_t loadBUart;
    if (!loadB.isEmpty() && loadB.size() > 15) {//load.size() == [16, 19]
        Sleep(700);
        loadBUart.data = loadB;
    } else {
        loadBUart.data = "";
    }
    loadBUart.com = comLoadB;
    devInformation << loadBUart;
    //loadC                 4
    splash->setLabelTest(QObject::tr("Checking the Load(ServoC) UART..."));
    splash->setProgress(500/10+(qrand()%10-5));

    QString loadC = CollectControl::HardSend(comLoadC, QString("RDM%101%102%\r").toLatin1().data(),1100);
    UART_t loadCUart;
    if (!loadC.isEmpty() && loadC.size() > 15) {//load.size() == [16, 19]
        Sleep(700);
        loadCUart.data = loadC;
    } else {
        loadCUart.data = "";
    }
    loadCUart.com = comLoadC;
    devInformation << loadCUart;
    //test servoA           5
    splash->setLabelTest(QObject::tr("Checking the Life Tester ServoA UART...."));
    splash->setProgress(600/10+(qrand()%10-5));

    QString testA = CollectControl::HardSend(comTest, QString("RDWD020603").toLatin1().data(),1100);
    UART_t testAUart;
    if (!testA.isEmpty() && testA.size() == 13) {
        Sleep(700);
        testAUart.data = testA;
    } else {
        testAUart.data = "";
    }
    testAUart.com = comTest;
    devInformation << testAUart;
    //test servoB           6
    splash->setLabelTest(QObject::tr("Checking the Life Tester ServoB UART..."));
    splash->setProgress(700/10+(qrand()%10-5));

    QString testB = CollectControl::HardSend(comTest, QString("RDWD030603").toLatin1().data(),1100);
    UART_t testBUart;
    if (!testB.isEmpty() && testB.size() == 13) {
        Sleep(700);
        testBUart.data = testB;
    } else {
        testBUart.data = "";
    }
    testBUart.com = comTest;
    devInformation << testBUart;
    //test servoC           7
    splash->setLabelTest(QObject::tr("Checking the Life Tester ServoC UART...."));
    splash->setProgress(800/10+(qrand()%10-5));

    QString testC = CollectControl::HardSend(comTest, QString("RDWD040603").toLatin1().data(),1100);
    UART_t testCUart;
    if (!testC.isEmpty() && testC.size() == 13) {
        Sleep(700);
        testCUart.data = testC;
    } else {
        testCUart.data = "";
    }
    testCUart.com = comTest;
    devInformation << testCUart;

    //temperature rise      8       //agilent 34970a  !!!!!待测试 命令 返回数据大小 判断正常条件
    splash->setLabelTest(QObject::tr("Checking the Agilent Paperless Recorder UART...."));
    splash->setProgress(900/10+(qrand()%10-5));

    QString trcr2 = CollectControl::HardSend(comTR_AGILENT34970, QString("FData,0,0001,0110").toLatin1().data(),1100);
    UART_t trcrUart2;
    if (!trcr2.isEmpty() && trcr2.size() == 723) {
        Sleep(700);
        trcrUart2.data = trcr2;
    } else {
        trcrUart2.data = "";
    }
    trcrUart2.com = comTR_AGILENT34970;
    devInformation << trcrUart2;

    if (connectSTAS) {
        splash->setLabelTest(QObject::tr("Connect STAS Sucess! Entering the login screen..."));
        splash->setProgress(100);
    } else {

        splash->setLabelTest(QObject::tr("Connect STAS failed! Entering the login screen...."));
        splash->setProgress(100);
    }
    Sleep(1600);

    w.show();
    w.move((QApplication::desktop()->width()-w.width())/2,
           (QApplication::desktop()->height()-w.height())/2);

    splash->finish(&w); //
    delete splash;

    return a.exec();
}
