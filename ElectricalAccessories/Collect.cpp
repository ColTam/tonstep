#include "Collect.h"
#include <QThread>
#include <QDebug>
#include <QSettings>

QList<UART_t> devInformation;
bool connectSTAS = false;
unsigned int comLoadA = 2;
unsigned int comLoadB = 1;
unsigned int comLoadC = 0;
unsigned int comVolt  = 4;
unsigned int comTRC   = 5;
unsigned int comTest  = 6;

bool Collect::IsTestServoA = false;
bool Collect::IsTestServoB = false;
bool Collect::IsTestServoC = false;

QString Collect::registrNumber = "";
QString Collect::voltageA = "";
QString Collect::currentA = "";
QString Collect::currentB = "";
QString Collect::currentC = "";
QString Collect::loadPowerFactorA = "";
QString Collect::loadPowerFactorB = "";
QString Collect::loadPowerFactorC = "";

//Collect::devState Collect::d_s;

Collect::Collect(QObject *parent) : QObject(parent)
{
}

bool Collect::ConnectToSTAS()
{
    QSettings *configIniRead = new QSettings("EATconfig.ini", QSettings::IniFormat);

    QString registrN = configIniRead->value("Registration Code").toString();
    delete configIniRead;

    if (!registrN.isEmpty()) {
        registrNumber = registrN;
    }

    return CollectControl::HardConnet(registrNumber.toLatin1().data());
    //    CollectControl::OnBnClickedButtonHome();
    //    CollectControl::OnBnClickedButtonClearlimit();
}

QString Collect::setLoadLPF275_6(int com_t, QString current)
{
    CollectControl::HardCommand(com_t, QString("SET%110%0%\r").toLatin1().data()); //set the L
/*    if (current == "2.5") {
        CollectControl::HardSend(com_t, QString("SET%102%3%\r").toLatin1().data());
        return "SET%101%30%\r";
    } else */if (current == "6") {
        CollectControl::HardCommand(com_t, QString("SET%102%9%\r").toLatin1().data());
        return "SET%101%73%\r";
    } else if (current == "20") {
        CollectControl::HardCommand(com_t, QString("SET%102%25%\r").toLatin1().data());
        return "SET%101%195%\r";
    }
    return QString("SET%101%%1%\r").arg(current.toDouble()*10+1);
}

QString Collect::setLoadLPF250_8(int com_t, QString current)
{
    CollectControl::HardCommand(com_t, QString("SET%110%0%\r").toLatin1().data()); //set the L
    if (current == "1") {
        CollectControl::HardCommand(com_t, QString("SET%102%2%\r").toLatin1().data());
        return "SET%101%11%\r";
    } else if (current == "2.5") {
        CollectControl::HardCommand(com_t, QString("SET%102%6%\r").toLatin1().data());
        return "SET%101%28%\r";
    } else if (current == "3") {
        CollectControl::HardCommand(com_t, QString("SET%102%7%\r").toLatin1().data());
        return "SET%101%34%\r";
    } else if (current == "4") {
        CollectControl::HardCommand(com_t, QString("SET%102%9%\r").toLatin1().data());
        return "SET%101%42%\r";
    } else if (current == "6") {
        CollectControl::HardCommand(com_t, QString("SET%102%15%\r").toLatin1().data());
        return "SET%101%63%\r";
    } else if (current == "10") {
        CollectControl::HardCommand(com_t, QString("SET%102%26%\r").toLatin1().data());
        return "SET%101%101%\r";
    } else if (current == "12") {
        CollectControl::HardCommand(com_t, QString("SET%102%32%\r").toLatin1().data());
        return "SET%101%122%\r";
    } else if (current == "13") {
        CollectControl::HardCommand(com_t, QString("SET%102%36%\r").toLatin1().data());
        return "SET%101%133%\r";
    } else if (current == "16") {
        CollectControl::HardCommand(com_t, QString("SET%102%48%\r").toLatin1().data());
        return "SET%101%164%\r";
    } else if (current == "22") {
        CollectControl::HardCommand(com_t, QString("SET%102%79%\r").toLatin1().data());
        return "SET%101%238%\r";
    } else if (current == "25") {
        CollectControl::HardCommand(com_t, QString("SET%102%109%\r").toLatin1().data());
        return "SET%101%265%\r";
    } else if (current == "31") {
        CollectControl::HardCommand(com_t, QString("SET%102%190%\r").toLatin1().data());
        return "SET%101%354%\r";
    } else if (current == "32") {
        CollectControl::HardCommand(com_t, QString("SET%102%192%\r").toLatin1().data());
        return "SET%101%369%\r";
    }
    return QString("SET%101%%1%\r").arg(current.toDouble()*10+1);
}

void Collect::ComponentsShortCut(const QString servo)
{
    if (servo == "A") {
        CollectControl::SetIoStatus(1, 7, 1);
    } else if (servo == "B") {
        CollectControl::SetIoStatus(2, 1, 1);
    } else if (servo == "C") {
        CollectControl::SetIoStatus(2, 0, 1);
    }
}

void Collect::ComponentsNoShortCut(const QString servo)
{
    if (servo == "A") {
        CollectControl::SetIoStatus(1, 7, 0);
    } else if (servo == "B") {
        CollectControl::SetIoStatus(2, 1, 0);
    } else if (servo == "C") {
        CollectControl::SetIoStatus(2, 0, 0);
    }
}

void Collect::CutToLN(const QString servo)
{
    if (servo == "A") {
        CollectControl::SetIoStatus(0, 4, 0);
    } else if (servo == "B") {
        CollectControl::SetIoStatus(0, 5, 0);
    } else if (servo == "C") {
        CollectControl::SetIoStatus(0, 6, 0);
    }
}

void Collect::CutToLE(const QString servo)
{
    if (servo == "A") {
        CollectControl::SetIoStatus(0, 4, 1);
    } else if (servo == "B") {
        CollectControl::SetIoStatus(0, 5, 1);
    } else if (servo == "C") {
        CollectControl::SetIoStatus(0, 6, 1);
    }
}

void Collect::SetVolt(const QString volt)
{
    CollectControl::HardSend(devInformation.at(0).com, QString("WRW P:0").toLatin1().data());
    QString _volt = "WRW VF%" + volt.left(3) + "0%500%";
//    qDebug() << volt << _volt << devInformation.at(0).com;
    voltageA = volt;
    CollectControl::HardCommand(devInformation.at(0).com, _volt.toLatin1().data());
    CollectControl::HardCommand(devInformation.at(0).com, _volt.toLatin1().data());
}

void Collect::SetLoad(const QString LPFactor, const QString current, const QString servo, bool record)
{
    QString _load;
    QString lpf;
    if (LPFactor == "104") {
        if (servo == "A") {
            _load = setLoadLPF275_6(devInformation.at(2).com, current);
        } else if (servo == "B") {
            _load = setLoadLPF275_6(devInformation.at(3).com, current);
        } else if (servo == "C") {
            _load = setLoadLPF275_6(devInformation.at(4).com, current);
        }
        lpf = "0.6";
    } else if (LPFactor == "108") {
        if (servo == "A") {
            _load = setLoadLPF250_8(devInformation.at(2).com, current);
        } else if (servo == "B") {
            _load = setLoadLPF250_8(devInformation.at(3).com, current);
        } else if (servo == "C") {
            _load = setLoadLPF250_8(devInformation.at(4).com, current);
        }
        lpf = "0.8";
    } else if (LPFactor == "106") {
        if (current.size() < 3) {
            if (current.left(2).toInt() > 16) {
                _load = "SET%106%" + current.left(2) + QString("%1%\r").arg(current.left(2).toInt()-16+3);
            } else {
                _load = "SET%106%" + current.left(2) + "1%\r";
            }
        } else {
            if (current.left(2).toInt() > 16) {
                _load = "SET%106%" + QString::number(current.left(4).toDouble()*10+current.left(2).toInt()-16+3) + "%\r";
            } else {
                _load = "SET%106%" + QString::number(current.left(4).toDouble()*10+1) + "%\r";
            }
        }
        lpf = "1.0";
    } else if (LPFactor == "109") {
        if (current.toInt() == 20) {//In == 16A
            _load = "SET%106%184%\r";
        } else {                    //In == 10A
            _load = "SET%106%112%\r";
        }
        lpf = "1.0";
    }

    if (servo == "A") {
        if (record) {
            currentA = current.left(4);
            loadPowerFactorA = lpf;
        }
        CollectControl::HardSend(devInformation.at(2).com, _load.toLatin1().data());
    } else if (servo == "B") {
        if (record) {
            currentB = current.left(4);
            loadPowerFactorB = lpf;
        }
        CollectControl::HardSend(devInformation.at(3).com, _load.toLatin1().data());
    } else if (servo == "C") {
        if (record) {
            currentC = current.left(4);
            loadPowerFactorC = lpf;
        }
        CollectControl::HardSend(devInformation.at(4).com, _load.toLatin1().data());
    }
}

void Collect::SetTestNumber(const QString number, const QString servo)
{
    if (servo == "A") {
        QString _number = "WRWD020601" + number;
        CollectControl::HardCommand(devInformation.at(5).com, _number.toLatin1().data());
    } else if (servo == "B") {
        QString _number = "WRWD030601" + number;
        CollectControl::HardCommand(devInformation.at(6).com, _number.toLatin1().data());
    } else if (servo == "C") {
        QString _number = "WRWD040601" + number;
        CollectControl::HardCommand(devInformation.at(7).com, _number.toLatin1().data());
    }
}

void Collect::SetTestCycleNumber(const QString number, const QString servo)
{
    if (servo == "A") {
        QString _number = "WRWD020801" + QString("%1").arg(number.toInt()/2,4,16,QLatin1Char('0')).toUpper();
        CollectControl::HardCommand(devInformation.at(5).com, _number.toLatin1().data());
    } else if (servo == "B") {
        QString _number = "WRWD030801" + QString("%1").arg(number.toInt()/2,4,16,QLatin1Char('0')).toUpper();
        CollectControl::HardCommand(devInformation.at(6).com, _number.toLatin1().data());
    } else if (servo == "C") {
        QString _number = "WRWD040801" + QString("%1").arg(number.toInt()/2,4,16,QLatin1Char('0')).toUpper();
        CollectControl::HardCommand(devInformation.at(7).com, _number.toLatin1().data());
    }
}

void Collect::SetTest(const QString number, const QString rate, const QString position, const QString time, const QString servo)
{
    if (servo == "A") {
        QString _rate1 = rate+"0";
        _rate1 = "WRWD020201" + QString("%1").arg(_rate1.toInt(),4,16,QLatin1Char('0')).toUpper();
        CollectControl::HardCommand(devInformation.at(5).com, _rate1.toLatin1().data());

        QString _rate2 = rate+"0";
        _rate2 = "WRWD021201" + QString("%1").arg(_rate2.toInt(),4,16,QLatin1Char('0')).toUpper();
        CollectControl::HardCommand(devInformation.at(5).com, _rate2.toLatin1().data());

        QString _position = position + "000";
        _position = "WRWD020001" + QString("%1").arg(_position.toInt(),4,16,QLatin1Char('0')).toUpper();
        CollectControl::HardCommand(devInformation.at(5).com, _position.toLatin1().data());

        float rate_15 = 800;
        float rate_30 = 400;
        QString _time1, _time2;
        if (time.toInt() == 15) {
            _time1 = "WRWD020401012C";
            _time2 = "WRWD021401" + QString("%1").arg(QString::number(rate_15-300-position.toInt()*200/rate.toInt()).toInt(),4,16,QLatin1Char('0')).toUpper();
        } else if (time.toInt() == 30) {
            _time1 = "WRWD0204010096";
            _time2 = "WRWD021401" + QString("%1").arg(QString::number(rate_30-150-position.toInt()*200/rate.toInt()).toInt(),4,16,QLatin1Char('0')).toUpper();
        }
        CollectControl::HardCommand(devInformation.at(5).com, _time1.toLatin1().data());
        CollectControl::HardCommand(devInformation.at(5).com, _time2.toLatin1().data());

        QString isRun = CollectControl::HardSend(devInformation.at(5).com, QString("RDWM011001").toLatin1().data());
        QString isReady = CollectControl::HardSend(devInformation.at(5).com, QString("RDWD020601").toLatin1().data());
        if (isRun.right(2).toInt(nullptr,16) > 0 && isReady.toInt(nullptr, 16) == 0){
            IsTestServoB = true;
        }
    } else if (servo == "B") {
        QString _rate1 = rate+"0";
        _rate1 = "WRWD030201" + QString("%1").arg(_rate1.toInt(),4,16,QLatin1Char('0')).toUpper();
        CollectControl::HardCommand(devInformation.at(6).com, _rate1.toLatin1().data());

        QString _rate2 = rate+"0";
        _rate2 = "WRWD031201" + QString("%1").arg(_rate2.toInt(),4,16,QLatin1Char('0')).toUpper();
        CollectControl::HardCommand(devInformation.at(6).com, _rate2.toLatin1().data());

        QString _position = position + "000";
        _position = "WRWD030001" + QString("%1").arg(_position.toInt(),4,16,QLatin1Char('0')).toUpper();
        CollectControl::HardCommand(devInformation.at(6).com, _position.toLatin1().data());

        float rate_15 = 800;
        float rate_30 = 400;
        QString _time1, _time2;
        if (time.toInt() == 15) {
            _time1 = "WRWD030401012C";
            _time2 = "WRWD031401" + QString("%1").arg(QString::number(rate_15-300-position.toInt()*200/rate.toInt()).toInt(),4,16,QLatin1Char('0')).toUpper();
        } else if (time.toInt() == 30) {
            _time1 = "WRWD0304010096";
            _time2 = "WRWD031401" + QString("%1").arg(QString::number(rate_30-150-position.toInt()*200/rate.toInt()).toInt(),4,16,QLatin1Char('0')).toUpper();
        }
        CollectControl::HardCommand(devInformation.at(6).com, _time1.toLatin1().data());
        CollectControl::HardCommand(devInformation.at(6).com, _time2.toLatin1().data());

        QString isRun = CollectControl::HardSend(devInformation.at(6).com, QString("RDWM012001").toLatin1().data());
        QString isReady = CollectControl::HardSend(devInformation.at(6).com, QString("RDWD030601").toLatin1().data());
        if (isRun.right(2).toInt(nullptr,16) > 0 && isReady.toInt(nullptr, 16) == 0){
            IsTestServoC = true;
        }
    } else if (servo == "C") {
        QString _rate1 = rate+"0";
        _rate1 = "WRWD040201" + QString("%1").arg(_rate1.toInt(),4,16,QLatin1Char('0')).toUpper();
        CollectControl::HardCommand(devInformation.at(7).com, _rate1.toLatin1().data());

        QString _rate2 = rate+"0";
        _rate2 = "WRWD041201" + QString("%1").arg(_rate2.toInt(),4,16,QLatin1Char('0')).toUpper();
        CollectControl::HardCommand(devInformation.at(7).com, _rate2.toLatin1().data());

        QString _position = position + "000";
        _position = "WRWD040001" + QString("%1").arg(_position.toInt(),4,16,QLatin1Char('0')).toUpper();
        CollectControl::HardCommand(devInformation.at(7).com, _position.toLatin1().data());

        float rate_15 = 800;
        float rate_30 = 400;
        QString _time1, _time2;
        if (time.toInt() == 15) {
            _time1 = "WRWD040401012C";
            _time2 = "WRWD041401" + QString("%1").arg(QString::number(rate_15-300-position.toInt()*200/rate.toInt()).toInt(),4,16,QLatin1Char('0')).toUpper();
        } else if (time.toInt() == 30) {
            _time1 = "WRWD0404010096";
            _time2 = "WRWD041401" + QString("%1").arg(QString::number(rate_30-150-position.toInt()*200/rate.toInt()).toInt(),4,16,QLatin1Char('0')).toUpper();
        }
        CollectControl::HardCommand(devInformation.at(7).com, _time1.toLatin1().data());
        CollectControl::HardCommand(devInformation.at(7).com, _time2.toLatin1().data());
    }

//    TestComeBackToOrigin(servo);
    SetTestCycleNumber(number, servo);
}

void Collect::PowerStart()
{
    CollectControl::HardCommand(devInformation.at(0).com, QString("WRW P:1").toLatin1().data());
}

void Collect::LoadStart(const QString servo)
{
    if (servo == "A") {
        CollectControl::HardSend(devInformation.at(2).com, QString("SET%107%1%\r").toLatin1().data());
    } else if (servo == "B") {
        CollectControl::HardSend(devInformation.at(3).com, QString("SET%107%1%\r").toLatin1().data());
    } else if (servo == "C") {
        CollectControl::HardSend(devInformation.at(4).com, QString("SET%107%1%\r").toLatin1().data());
    }
}

void Collect::TestStart(const QString servo)
{
    if (servo == "A") {
        IsTestServoA = true;
        if (IsTestServoB) {
            CollectControl::HardCommand(devInformation.at(5).com, QString("WRWM0100010401").toLatin1().data());
        } else {
            CollectControl::HardCommand(devInformation.at(5).com, QString("WRWM0100010001").toLatin1().data());
        }
    } else if (servo == "B") {
        IsTestServoB = true;
        if (IsTestServoC) {
            CollectControl::HardCommand(devInformation.at(6).com, QString("WRWM0110010401").toLatin1().data());
        } else {
            CollectControl::HardCommand(devInformation.at(6).com, QString("WRWM0110010001").toLatin1().data());
        }
    } else if (servo == "C"){
        IsTestServoC = true;
        CollectControl::HardCommand(devInformation.at(7).com, QString("WRWM0120010001").toLatin1().data());
    }
}

void Collect::PowerStop()
{
    CollectControl::HardSend(devInformation.at(0).com, QString("WRW P:0").toLatin1().data());
}

void Collect::LoadStop(const QString servo)
{
    if (servo == "A") {
        CollectControl::HardSend(devInformation.at(2).com, QString("SET%107%0%\r").toLatin1().data());
    } else if (servo == "B") {
        CollectControl::HardSend(devInformation.at(3).com, QString("SET%107%0%\r").toLatin1().data());
    } else if (servo == "C") {
        CollectControl::HardSend(devInformation.at(4).com, QString("SET%107%0%\r").toLatin1().data());
    }
}

void Collect::TestStop(const QString servo)
{
    if (servo == "A") {
        IsTestServoA = false;
        if (IsTestServoB) {
            CollectControl::HardCommand(devInformation.at(5).com, QString("WRWM0100010400").toLatin1().data());
        } else {
            CollectControl::HardCommand(devInformation.at(5).com, QString("WRWM0100010000").toLatin1().data());
        }
    } else if (servo == "B") {
        IsTestServoB = false;
        if (IsTestServoC) {
            CollectControl::HardCommand(devInformation.at(6).com, QString("WRWM0110010400").toLatin1().data());
        } else {
            CollectControl::HardCommand(devInformation.at(6).com, QString("WRWM0110010000").toLatin1().data());
        }
    } else if (servo == "C"){
        IsTestServoC = false;
        CollectControl::HardCommand(devInformation.at(7).com, QString("WRWM0120010000").toLatin1().data());
    }
}

QString Collect::GetTestNumber(const QString servo)
{
    if (servo == "A") {
        return CollectControl::HardSend(devInformation.at(5).com, QString("RDWD020601").toLatin1().data());
    } else if (servo == "B") {
        return CollectControl::HardSend(devInformation.at(6).com, QString("RDWD030601").toLatin1().data());
    } else if (servo == "C") {
        return CollectControl::HardSend(devInformation.at(7).com, QString("RDWD040601").toLatin1().data());
    }

    return NULL;
}

QString Collect::GetTestPosition(const QString servo)
{
    if (servo == "A") {
        return CollectControl::HardSend(devInformation.at(5).com, QString("RDWD003001").toLatin1().data());
    } else if (servo == "B") {
        return CollectControl::HardSend(devInformation.at(6).com, QString("RDWD003201").toLatin1().data());
    } else if (servo == "C") {
        return CollectControl::HardSend(devInformation.at(7).com, QString("RDWD003401").toLatin1().data());
    }

    return NULL;
}

void Collect::TestComeBackToOrigin(const QString servo)
{
    if (servo == "A") {
        CollectControl::HardSend(devInformation.at(5).com, QString("WRWM0050010001").toLatin1().data());
    } else if (servo == "B") {
        CollectControl::HardSend(devInformation.at(6).com, QString("WRWM0052010001").toLatin1().data());
    } else if (servo == "C") {
        CollectControl::HardSend(devInformation.at(7).com, QString("WRWM0054010001").toLatin1().data());
    }
}

void Collect::TestClearNumber(const QString servo)
{
    if (servo == "A") {
        CollectControl::HardSend(devInformation.at(5).com, QString("WRWD0206010000").toLatin1().data());
    } else if (servo == "B") {
        CollectControl::HardSend(devInformation.at(6).com, QString("WRWD0306010000").toLatin1().data());
    } else if (servo == "C") {
        CollectControl::HardSend(devInformation.at(7).com, QString("WRWD0406010000").toLatin1().data());
    }
}

void Collect::AlarmOpen()
{
    CollectControl::SetIoStatus(0, 7, 1);
}

void Collect::AlarmClose()
{
    CollectControl::SetIoStatus(0, 7, 0);
}

void Collect::TestAlarmClose(QString servo)
{
    if (servo == "A") {
        CollectControl::HardSend(devInformation.at(5).com, QString("WRWD0206010000").toLatin1().data());
    } else if (servo == "B") {
        CollectControl::HardSend(devInformation.at(6).com, QString("WRWD0306010000").toLatin1().data());
    } else if (servo == "C") {
        CollectControl::HardSend(devInformation.at(7).com, QString("WRWD0406010000").toLatin1().data());
    }
}

void Collect::TestDisplayOpen()
{
    CollectControl::SetIoStatus(2, 2, 0);
}

void Collect::TestDisplayClose()
{
    CollectControl::SetIoStatus(2, 2, 1);
}

void Collect::getTemperatureRise(QStringList *list)
{
    int i = 0;
    while (1)
    {
        if (i == list->count()) break;

        QString str = list->at(i);
        if (str.size() == 32) {//N 0101    掳C       +00000374E-01
            ++i;
            continue;
        } else {
            list->removeAt(i);
            continue;
        }
    }
}

void Collect::stateStand()
{
    CollectControl::SetIoStatus(1, 5, 0);
    CollectControl::SetIoStatus(1, 6, 0);
    CollectControl::SetIoStatus(1, 4, 1);
    Collect::AlarmClose();
}

void Collect::stateRunning()
{
    CollectControl::SetIoStatus(1, 4, 0);
    CollectControl::SetIoStatus(1, 6, 0);
    CollectControl::SetIoStatus(1, 5, 1);
    Collect::AlarmClose();
}

void Collect::stateWarning()
{
    CollectControl::SetIoStatus(1, 4, 0);
    CollectControl::SetIoStatus(1, 5, 0);
    CollectControl::SetIoStatus(1, 6, 1);
    Collect::AlarmOpen();
}

void Collect::stateAllStop()
{
    CollectControl::SetIoStatus(1, 4, 0);
    CollectControl::SetIoStatus(1, 5, 0);
    CollectControl::SetIoStatus(1, 6, 0);
    Collect::AlarmClose();
}
