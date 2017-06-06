#include "widget.h"
#include "mtimer.h"
#include "mCRC16.h"
#include "charts/mchart.h"

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLegend>
#include <QBitArray>
#include <QFileDialog>
#include <QThread>
#include <QTime>

int Widget::mTimeoutCount = 0;

Widget::Widget(QWidget *parent)
    : QWidget(parent),
      m_stateLabel(createState()),
      m_initatingPressureLineEdit(createWriteLineEdit()),
      m_stageStepUpLineEdit(createWriteLineEdit()),
      m_stageTimeLineEdit(createWriteLineEdit()),
      m_stepUpNumberLineEdit(createWriteLineEdit()),
      m_pressureDropLineEdit(createWriteLineEdit()),
      m_testPatternLineEdit(createWriteLineEdit()),
      m_currentPressureLineEdit(createReadLineEdit()),
      m_maxPressureLineEdit(createReadLineEdit()),
      m_writeArgumentPushButton(new QPushButton(tr("设置参数"))),
      m_readArgumentPushButton(new QPushButton(tr("读取参数"))),
      m_startPushButton(new QPushButton(tr("启动"))),
      m_stopPushButton(new QPushButton(tr("停止"))),
      m_savePushButton(new QPushButton(tr("保存"))),
      m_on_off_uartPushButton(new QPushButton(tr("打开"))),
      m_uartComboBox(createUartComboBox()),
      m_chart(createSplineChart()),
      m_chartView(new QChartView(m_chart)),
      m_serialPort(nullptr),
      mNewData(QString::null),
      mData(QString::null),
      getArgument(false),
      getPressure(false),
      m_timer(new QTimer())
{
    connectSignals();

    QVBoxLayout *topVLayout1 = new QVBoxLayout();
    topVLayout1->addWidget(new QLabel(tr("起始压气")));
    topVLayout1->addWidget(m_initatingPressureLineEdit);
    topVLayout1->addWidget(new QLabel(tr("参值0-50")));

    QVBoxLayout *topVLayout2 = new QVBoxLayout();
    topVLayout2->addWidget(new QLabel(tr("阶段升压")));
    topVLayout2->addWidget(m_stageStepUpLineEdit);
    topVLayout2->addWidget(new QLabel(tr("参值5-50")));

    QVBoxLayout *topVLayout3 = new QVBoxLayout();
    topVLayout3->addWidget(new QLabel(tr("阶段时间")));
    topVLayout3->addWidget(m_stageTimeLineEdit);
    topVLayout3->addWidget(new QLabel(tr("参值1-50")));

    QVBoxLayout *topVLayout4 = new QVBoxLayout();
    topVLayout4->addWidget(new QLabel(tr("升压次数")));
    topVLayout4->addWidget(m_stepUpNumberLineEdit);
    topVLayout4->addWidget(new QLabel(tr("参值1-99")));

    QVBoxLayout *topVLayout5 = new QVBoxLayout();
    topVLayout5->addWidget(new QLabel(tr("极限降压")));
    topVLayout5->addWidget(m_pressureDropLineEdit);
    topVLayout5->addWidget(new QLabel(tr("参值1-99")));

    QVBoxLayout *topVLayout6 = new QVBoxLayout();
    topVLayout6->addWidget(new QLabel(tr("实验模式")));
    topVLayout6->addWidget(m_testPatternLineEdit);
    topVLayout6->addWidget(new QLabel(tr("参值0/1")));

    QHBoxLayout *topHLayout = new QHBoxLayout();
    topHLayout->addSpacing(20);
    topHLayout->addStretch();
    topHLayout->addLayout(topVLayout1);
    topHLayout->addSpacing(20);
    topHLayout->addLayout(topVLayout2);
    topHLayout->addSpacing(20);
    topHLayout->addLayout(topVLayout3);
    topHLayout->addSpacing(20);
    topHLayout->addLayout(topVLayout4);
    topHLayout->addSpacing(20);
    topHLayout->addLayout(topVLayout5);
    topHLayout->addSpacing(20);
    topHLayout->addLayout(topVLayout6);
    topHLayout->addStretch();
    topHLayout->addSpacing(20);

    QHBoxLayout *midHLayout = new QHBoxLayout();
    midHLayout->addSpacing(10);
    midHLayout->addWidget(new QLabel(tr("当前气压:")));
    midHLayout->addWidget(m_currentPressureLineEdit);
    midHLayout->addStretch();
    midHLayout->addWidget(new QLabel(tr("串口号:")));
    midHLayout->addWidget(m_uartComboBox);
    midHLayout->addWidget(m_on_off_uartPushButton);
    midHLayout->addWidget(m_writeArgumentPushButton);
    midHLayout->addWidget(m_readArgumentPushButton);
    midHLayout->addSpacing(10);

    m_chartView->setObjectName("mChartView");
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->chart()->setTheme(QChart::ChartThemeBlueIcy);
//    m_chartView->setStyleSheet("background: transparent;");

    QHBoxLayout *bottomHLayout = new QHBoxLayout();
    bottomHLayout->addSpacing(20);
    bottomHLayout->addWidget(new QLabel(tr("系统状态:")));
    bottomHLayout->addWidget(m_stateLabel);
    bottomHLayout->addSpacing(20);
    bottomHLayout->addWidget(new QLabel(tr("极限耐压:")));
    bottomHLayout->addWidget(m_maxPressureLineEdit);
    bottomHLayout->addStretch();
    bottomHLayout->addWidget(m_startPushButton);
    bottomHLayout->addWidget(m_stopPushButton);
    bottomHLayout->addSpacing(30);
    bottomHLayout->addWidget(m_savePushButton);
    bottomHLayout->addSpacing(10);

    QGridLayout *baseLayout = new QGridLayout();
    baseLayout->addLayout(topHLayout, 0, 0, 1, 1);
    baseLayout->addWidget(new QLabel(""), 1, 0, 1, 1);
    baseLayout->addLayout(midHLayout, 2, 0);
    baseLayout->addWidget(m_chartView, 3, 0);
    baseLayout->addLayout(bottomHLayout, 4, 0);

    m_startPushButton->setEnabled(false);
    m_stopPushButton->setEnabled(false);
    setLayout(baseLayout);

    connect(m_timer, &QTimer::timeout, this, &Widget::m_timerout);
}

Widget::~Widget()
{
}
//draw the background-image
void Widget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawPixmap(0,0,this->width(),this->height(),QPixmap(":/image/icon/bg1"));
}

QLineEdit *Widget::createWriteLineEdit() const
{
    QLineEdit *lineEdit = new QLineEdit();
    lineEdit->setPlaceholderText("0");
    lineEdit->setMaximumWidth(5*FONTSIZE+3);
    lineEdit->setMaxLength(2);
    lineEdit->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    return lineEdit;
}

QLineEdit *Widget::createReadLineEdit() const
{
    QLineEdit *lineEdit = new QLineEdit();
    lineEdit->setPlaceholderText("0 PSL");
    lineEdit->setMaximumWidth(5*FONTSIZE+8);
    lineEdit->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    lineEdit->setReadOnly(true);

    return lineEdit;
}

void Widget::connectSignals()
{
    connect(m_on_off_uartPushButton, SIGNAL(clicked(bool)), this, SLOT(openUartSlot()));
    connect(m_writeArgumentPushButton, SIGNAL(clicked(bool)), this, SLOT(writeArgumentSlot()));
    connect(m_readArgumentPushButton, &QPushButton::clicked, [this]() {
        getArgument = true;
    });
    connect(m_startPushButton, &QPushButton::clicked, [this]() {
        if (m_serialPort != nullptr) {
            mTimeoutCount = 0;
            mNewData.clear();
            m_startPushButton->setEnabled(false);
            m_stopPushButton->setEnabled(true);
            m_stateLabel->setPixmap(QPixmap(":/image/icon/on"));

            QByteArray data;//启动测试机
            String2Hex("0A 06 00 07 00 01 F8 B0", data);
            m_serialPort->write(data);
        } else {
            QMessageBox msgBox(QMessageBox::Warning,tr("错误"),tr("未检测到可用串口."));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setButtonText(QMessageBox::Ok,tr("确定"));

            msgBox.exec();
        }
    });
    connect(m_stopPushButton, &QPushButton::clicked, [this]() {
        if (m_serialPort != nullptr) {
            mTimeoutCount = 0;
            mNewData.clear();
            if (getPressure) {
                QByteArray data;//停止测试机
                String2Hex("0A 06 00 07 00 00 39 70", data);
                m_serialPort->write(data);
            }

            m_stopPushButton->setEnabled(false);
            m_startPushButton->setEnabled(true);
            m_stateLabel->setPixmap(QPixmap(":/image/icon/off"));
        } else {
            QMessageBox msgBox(QMessageBox::Warning,tr("错误"),tr("未检测到可用串口."));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setButtonText(QMessageBox::Ok,tr("确定"));

            msgBox.exec();
        }
    });
    connect(m_savePushButton, &QPushButton::clicked, [this](){
        qDebug() << this->size();
        QString saveFileName = QFileDialog::getSaveFileName(nullptr, tr("保存"), "./", tr("Image (*.png)"));
        if (!saveFileName.isEmpty()) {
            emit m_dataSave(saveFileName);
        } else {
            QMessageBox msgBox(QMessageBox::Warning,tr("错误"),tr("文件名无效."));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setButtonText(QMessageBox::Ok,tr("确定"));

            msgBox.exec();
        }
    });
}

QChart *Widget::createSplineChart() const
{
    // spine chart
    mChart *chart = new mChart;
//        chart->setTitle(tr("SplineSeries"));
    chart->legend()->hide();
    chart->resize(900,500);
    chart->setAnimationOptions(QChart::NoAnimation);

    connect(this, SIGNAL(m_dataSave(QString)), chart, SLOT(saveDataSlot(QString)));
    connect(chart, SIGNAL(savep(QString)), this, SLOT(savepn(QString)));
    connect(this, SIGNAL(updatePressure(QString)), chart, SLOT(paintPressure(QString)));
    connect(m_startPushButton, &QPushButton::clicked, chart, &mChart::splineClear);

    return chart;
}

QLabel *Widget::createState() const
{
    QLabel *label = new QLabel();
    label->setPixmap(QPixmap(":/image/icon/off"));

    return label;
}

QComboBox *Widget::createUartComboBox() const
{
    QComboBox *box = new QComboBox();
    box->setMinimumSize(box->minimumSizeHint());

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QSerialPort serialPort;
        serialPort.setPort(info);

        if (serialPort.open(QIODevice::ReadWrite)) {
            box->addItem(info.portName());
            serialPort.close();
        }
    }

    return box;
}

//保存曲线图
void Widget::savepn(const QString &file)
{
    QPixmap pix = this->grab(QRect(m_chartView->geometry()));
    if (pix.save(file,"png"))
        qDebug() << "secces";
}
//打开串口
void Widget::openUartSlot()
{
    if (m_uartComboBox->count()<1) {
        QMessageBox msgBox(QMessageBox::Warning,tr("错误"),tr("未检测到可用串口."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok,tr("确定"));

        msgBox.exec();
        return;
    }

    if (m_on_off_uartPushButton->text() == "打开"){
        //串口设置
        m_serialPort = new QSerialPort;//创建QSerialPort对象
        m_serialPort->setPortName(m_uartComboBox->currentText());//设置串口名
        if (!m_serialPort->open(QIODevice::ReadWrite)) {//打开串口
//            QMessageBox::warning(NULL, "warning", "SerialPort Open Failed");
//            return;

            QMessageBox msgBox(QMessageBox::Warning,tr("错误"),tr("串口:%1打开失败!").arg(m_serialPort->portName()));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setButtonText(QMessageBox::Ok,tr("确定"));

            msgBox.exec();
            return;
        }
        m_serialPort->setBaudRate(QSerialPort::Baud115200);//设置串口波特率
        m_serialPort->setDataBits(QSerialPort::Data8);//设置串口数据位
        m_serialPort->setParity(QSerialPort::NoParity);//设置串口校验位
        m_serialPort->setStopBits(QSerialPort::OneStop);//设置串口停止位
        m_serialPort->setFlowControl(QSerialPort::NoFlowControl);//设置流控制
        connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(readArgumentSlot()));
        //界面设置
        m_on_off_uartPushButton->setText(tr("关闭"));
        m_startPushButton->setEnabled(true);

        if (!m_timer->isActive())
            m_timer->start(100);

        getArgument = true;
    } else {
        if (m_timer->isActive())
            m_timer->stop();

        m_serialPort->close();
        delete m_serialPort;
        m_serialPort = nullptr;

        m_on_off_uartPushButton->setText(tr("打开"));
        m_startPushButton->setEnabled(false);
    }
}
//向串口写入数据--设置参数
void Widget::writeArgumentSlot()
{
    if (!m_serialPort->isOpen()){
//        qDebug() << tr("SerialPort not open");
        QMessageBox msgBox(QMessageBox::Warning,tr("错误"),tr("未检测到可用串口."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok,tr("确定"));

        msgBox.exec();
        return;
    }

    {// 0.
        char ch[] = {0x0a, 0x06, 0x00, 0x00, 0x00, 0x00};
        QString dataBit = QString::number(m_initatingPressureLineEdit->text().toInt(), 16);

        if (dataBit.toInt(nullptr, 16) > 50) {
            dataBit = QString::number(50, 16);
        } else if (dataBit.toInt(nullptr, 16) <= 0) {
            dataBit = "00";
        }
        m_initatingPressureLineEdit->setText(QString::number(dataBit.toInt(nullptr,16)));
        if (dataBit.size() == 1)
            dataBit.prepend("0");

        ch[5] = toAscii(dataBit.toInt(nullptr, 16));
        QString checkBit = QString::number(CRC16(ch, 6), 16);
        QString dataByte = QString("0a 06 00 00 00 %1 %2 %3").arg(
                dataBit,checkBit.mid(2,2),checkBit.mid(0,2));

        QByteArray data;
        String2Hex(dataByte, data);

        if (m_serialPort->isOpen())
            m_serialPort->write(data);
    }

    {// 1.
        char ch[] = {0x0a, 0x06, 0x00, 0x01, 0x00, 0x00};
        QString dataBit = QString::number(m_stageStepUpLineEdit->text().toInt(), 16);

        if (dataBit.toInt(nullptr, 16) > 50) {
            dataBit = QString::number(50, 16);
        } else if (dataBit.toInt(nullptr, 16) < 5) {
            dataBit = "05";
        }
        m_stageStepUpLineEdit->setText(QString::number(dataBit.toInt(nullptr,16)));
        if (dataBit.size() == 1)
            dataBit.prepend("0");

        ch[5] = toAscii(dataBit.toInt(nullptr, 16));
        QString checkBit = QString::number(CRC16(ch, 6), 16);
        QString dataByte = QString("0a 06 00 01 00 %1 %2 %3").arg(
                dataBit,checkBit.mid(2,2),checkBit.mid(0,2));

        QByteArray data;
        String2Hex(dataByte, data);

        if (m_serialPort->isOpen())
            m_serialPort->write(data);
    }

    {// 2.
        char ch[] = {0x0a, 0x06, 0x00, 0x02, 0x00, 0x00};
        QString dataBit = QString::number(m_stageTimeLineEdit->text().toInt(), 16);

        if (dataBit.toInt(nullptr, 16) > 50) {
            dataBit = QString::number(50, 16);
        } else if (dataBit.toInt(nullptr, 16) < 1) {
            dataBit = "01";
        }
        m_stageTimeLineEdit->setText(QString::number(dataBit.toInt(nullptr,16)));
        if (dataBit.size() == 1)
            dataBit.prepend("0");

        ch[5] = toAscii(dataBit.toInt(nullptr, 16));
        QString checkBit = QString::number(CRC16(ch, 6), 16);
        QString dataByte = QString("0a 06 00 02 00 %1 %2 %3").arg(
                dataBit,checkBit.mid(2,2),checkBit.mid(0,2));

        QByteArray data;
        String2Hex(dataByte, data);

        if (m_serialPort->isOpen())
            m_serialPort->write(data);
    }

    {// 3.
        char ch[] = {0x0a, 0x06, 0x00, 0x03, 0x00, 0x00};
        QString dataBit = QString::number(m_stepUpNumberLineEdit->text().toInt(), 16);

        if (dataBit.toInt(nullptr, 16) > 99) {
            dataBit = QString::number(99, 16);
        } else if (dataBit.toInt(nullptr, 16) < 1) {
            dataBit = "01";
        }
        m_stepUpNumberLineEdit->setText(QString::number(dataBit.toInt(nullptr,16)));
        if (dataBit.size() == 1)
            dataBit.prepend("0");

        ch[5] = toAscii(dataBit.toInt(nullptr, 16));
        QString checkBit = QString::number(CRC16(ch, 6), 16);
        QString dataByte = QString("0a 06 00 03 00 %1 %2 %3").arg(
                dataBit,checkBit.mid(2,2),checkBit.mid(0,2));

        QByteArray data;
        String2Hex(dataByte, data);

        if (m_serialPort->isOpen())
            m_serialPort->write(data);
    }

    {// 4.
        char ch[] = {0x0a, 0x06, 0x00, 0x04, 0x00, 0x00};
        QString dataBit = QString::number(m_pressureDropLineEdit->text().toInt(), 16);

        if (dataBit.toInt(nullptr, 16) > 99) {
            dataBit = QString::number(99, 16);
        } else if (dataBit.toInt(nullptr, 16) < 1) {
            dataBit = "01";
        }
        m_pressureDropLineEdit->setText(QString::number(dataBit.toInt(nullptr,16)));
        if (dataBit.size() == 1)
            dataBit.prepend("0");

        ch[5] = toAscii(dataBit.toInt(nullptr, 16));
        QString checkBit = QString::number(CRC16(ch, 6), 16);
        QString dataByte = QString("0a 06 00 04 00 %1 %2 %3").arg(
                dataBit,checkBit.mid(2,2),checkBit.mid(0,2));

        QByteArray data;
        String2Hex(dataByte, data);

        if (m_serialPort->isOpen())
            m_serialPort->write(data);
    }

    {// 5.
        char ch[] = {0x0a, 0x06, 0x00, 0x05, 0x00, 0x00};
        QString dataBit = QString::number(m_testPatternLineEdit->text().toInt(), 16);

        if (dataBit.toInt(nullptr, 16) > 1) {
            dataBit = "01";
        } else if (dataBit.toInt(nullptr, 16) < 1) {
            dataBit = "00";
        }
        m_testPatternLineEdit->setText(QString::number(dataBit.toInt(nullptr,16)));
        if (dataBit.size() == 1)
            dataBit.prepend("0");

        ch[5] = toAscii(dataBit.toInt(nullptr, 16));
        QString checkBit = QString::number(CRC16(ch, 6), 16);
        QString dataByte = QString("0a 06 00 05 00 %1 %2 %3").arg(
                dataBit,checkBit.mid(2,2),checkBit.mid(0,2));

        QByteArray data;
        String2Hex(dataByte, data);

        if (m_serialPort->isOpen())
            m_serialPort->write(data);
    }
}
//串口读取到的数据
void Widget::readArgumentSlot()
{
    QByteArray temp = m_serialPort->readAll();
    QDataStream out(&temp,QIODevice::ReadWrite);    //将字节数组读入

    while(!out.atEnd())
    {
        qint8 outChar = 0;
        out>>outChar;   //每字节填充一次，直到结束
        //十六进制的转换
        QString str = QString("%1").arg(outChar & 0xFF,2,16,QLatin1Char('0')).toUpper() + " ";
        mNewData += str;
    }

    if (mNewData.size() == 75) {
        mData = mNewData;
        mNewData.clear();
    } else if (mNewData.size() > 75){
        mNewData.clear();
    }

    if (++mTimeoutCount >= 2) {
        dealData(mData);
        mTimeoutCount=0;
    }
}
//实时接收数据
void Widget::m_timerout()
{
    if (m_serialPort->isOpen()) {
        QByteArray data;
        String2Hex("0A 03 00 00 00 0A C4 B6", data);
        m_serialPort->write(data);
    } else {
        QMessageBox msgBox(QMessageBox::Warning,tr("错误"),tr("未检测到可用串口."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok,tr("确定"));

        msgBox.exec();
        return;
    }
}
//引用他人已实现函数 串口以16进制发送数据字符串转换函数
void Widget::String2Hex(const QString &str, QByteArray &senddata)
{
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;
    for(int i=0; i<len; )
    {
        //char lstr,
        hstr=str[i].toLatin1();//qt5.8版本代替.toAscii()的函数
        if(hstr == ' ') {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
}

char Widget::ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}
//数据处理函数
void Widget::dealData(const QString &str)
{
    getPressure = str.mid(55,1).toInt();
//    qDebug() << str << str.size();

    m_maxPressureLineEdit->setText(QString("%1 PSL").arg((str.mid(45,2)+str.mid(48,2)).toInt(nullptr,16)));
    m_currentPressureLineEdit->setText(QString("%1 PSL").arg((str.mid(57,2)+str.mid(60,2)).toInt(nullptr,16)));//54 60 66

    if (getArgument) {
        m_initatingPressureLineEdit->setText(QString("%1").arg(str.mid(12,2).toInt(nullptr, 16)));
        m_stageStepUpLineEdit->setText(QString("%1").arg(str.mid(18,2).toInt(nullptr, 16)));
        m_stageTimeLineEdit->setText(QString("%1").arg(str.mid(24,2).toInt(nullptr, 16)));
        m_stepUpNumberLineEdit->setText(QString("%1").arg(str.mid(30,2).toInt(nullptr, 16)));
        m_pressureDropLineEdit->setText(QString("%1").arg(str.mid(36,2).toInt(nullptr, 16)));
        m_testPatternLineEdit->setText(QString("%1").arg(str.mid(42,2).toInt(nullptr, 16)));
        getArgument = !getArgument;
    }

    if (getPressure) {
        emit updatePressure(str.mid(57,2)+str.mid(60,2));
    } else {
        m_stopPushButton->click();
    }
}
