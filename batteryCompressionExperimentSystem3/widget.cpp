#include "widget.h"
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

Widget::Widget(QWidget *parent)
    : QWidget(parent),
      m_initatingPressureLineEdit(createWriteLineEdit()),
      m_stageStepUpLineEdit(createWriteLineEdit()),
      m_stageTimeLineEdit(createWriteLineEdit()),
      m_stepUpNumberLineEdit(createWriteLineEdit()),
      m_pressureDropLineEdit(createWriteLineEdit()),
      m_testPatternLineEdit(createWriteLineEdit()),
      m_maxPressureLineEdit(createReadLineEdit()),
      m_writeArgumentPushButton(createPushButton(tr("设置参数"))),
      m_readArgumentPushButton(createPushButton(tr("读取参数"))),
      m_startPushButton(createPushButton(tr("启动"))),
      m_stopPushButton(createPushButton(tr("停止"))),
      m_savePushButton(createPushButton(tr("保存"))),
      m_on_off_uartPushButton(createPushButton(tr("打开"))),
      m_themeComboBox(createThemeComboBox()),
      m_uartComboBox(createUartComboBox()),
      m_serialPort(nullptr)
{
    connectSignals();

    QVBoxLayout *topVLayout1 = new QVBoxLayout();
    topVLayout1->addWidget(createLabel(tr("起始压气")));
    topVLayout1->addWidget(m_initatingPressureLineEdit);
    topVLayout1->addWidget(createLabel(tr("参值0-50")));

    QVBoxLayout *topVLayout2 = new QVBoxLayout();
    topVLayout2->addWidget(createLabel(tr("阶段升压")));
    topVLayout2->addWidget(m_stageStepUpLineEdit);
    topVLayout2->addWidget(createLabel(tr("参值5-50")));

    QVBoxLayout *topVLayout3 = new QVBoxLayout();
    topVLayout3->addWidget(createLabel(tr("阶段时间")));
    topVLayout3->addWidget(m_stageTimeLineEdit);
    topVLayout3->addWidget(createLabel(tr("参值1-50")));

    QVBoxLayout *topVLayout4 = new QVBoxLayout();
    topVLayout4->addWidget(createLabel(tr("升压次数")));
    topVLayout4->addWidget(m_stepUpNumberLineEdit);
    topVLayout4->addWidget(createLabel(tr("参值1-99")));

    QVBoxLayout *topVLayout5 = new QVBoxLayout();
    topVLayout5->addWidget(createLabel(tr("极限降压")));
    topVLayout5->addWidget(m_pressureDropLineEdit);
    topVLayout5->addWidget(createLabel(tr("参值1-99")));

    QVBoxLayout *topVLayout6 = new QVBoxLayout();
    topVLayout6->addWidget(createLabel(tr("实验模式")));
    topVLayout6->addWidget(m_testPatternLineEdit);
    topVLayout6->addWidget(createLabel(tr("参值0/1")));

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
    midHLayout->addWidget(new QLabel(tr("Theme:")));
    midHLayout->addWidget(m_themeComboBox);
    midHLayout->addStretch();
    midHLayout->addWidget(new QLabel(tr("串口号:")));
    midHLayout->addWidget(m_uartComboBox);
    midHLayout->addWidget(m_on_off_uartPushButton);
    midHLayout->addWidget(m_writeArgumentPushButton);
    midHLayout->addWidget(m_readArgumentPushButton);
    midHLayout->addSpacing(10);

    c = createSplineChart();
    chartView = new QChartView(c);
    chartView->setRenderHint(QPainter::Antialiasing);


    QHBoxLayout *bottomHLayout = new QHBoxLayout();
    bottomHLayout->addSpacing(20);
    bottomHLayout->addWidget(new QLabel(tr("系统状态:")));
    bottomHLayout->addWidget(createState());
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
    baseLayout->addWidget(chartView, 3, 0);
    baseLayout->addLayout(bottomHLayout, 4, 0);

    setLayout(baseLayout);
    QFont font;
    font.setPointSize(FONTSIZE);
    this->setFont(font);
}

Widget::~Widget()
{
}

QLabel *Widget::createLabel(QString name) const
{
    QLabel *label = new QLabel();
    label->setText(name);
    label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    QFont font;
    font.setPointSize(FONTSIZE);
    label->setFont(font);

    return label;
}

QLineEdit *Widget::createWriteLineEdit() const
{
    QLineEdit *lineEdit = new QLineEdit();
    lineEdit->setPlaceholderText("0");
    lineEdit->setMaximumWidth(5*FONTSIZE+3);
    lineEdit->setMaxLength(2);
    lineEdit->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    QFont font;
    font.setPointSize(FONTSIZE);
    lineEdit->setFont(font);

    return lineEdit;
}

QLineEdit *Widget::createReadLineEdit() const
{
    QLineEdit *lineEdit = new QLineEdit();
    lineEdit->setPlaceholderText("0 PSL");
    lineEdit->setMaximumWidth(5*FONTSIZE+3);
    lineEdit->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    lineEdit->setReadOnly(true);

    return lineEdit;
}

QPushButton *Widget::createPushButton(QString name) const
{
    QPushButton *pushButton = new QPushButton();
    pushButton->setText(name);

    return pushButton;
}

QComboBox *Widget::createThemeComboBox() const
{
    QComboBox *comboBox = new QComboBox();
    comboBox->addItem("Light", QChart::ChartThemeLight);
    comboBox->addItem("Blue Cerulean", QChart::ChartThemeBlueCerulean);
    comboBox->addItem("Dark", QChart::ChartThemeDark);
    comboBox->addItem("Brown Sand", QChart::ChartThemeBrownSand);
    comboBox->addItem("Blue NCS", QChart::ChartThemeBlueNcs);
    comboBox->addItem("High Contrast", QChart::ChartThemeHighContrast);
    comboBox->addItem("Blue Icy", QChart::ChartThemeBlueIcy);
    comboBox->addItem("Qt", QChart::ChartThemeQt);
    return comboBox;
}

void Widget::connectSignals()
{
    connect(m_themeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(themeChanged()));
    connect(m_on_off_uartPushButton, SIGNAL(clicked(bool)), this, SLOT(openUartSlot()));
    connect(m_writeArgumentPushButton, SIGNAL(clicked(bool)), this, SLOT(writeArgumentSlot()));
    connect(m_readArgumentPushButton, SIGNAL(clicked(bool)), this, SLOT(readArgumentSlot()));
    connect(m_startPushButton, SIGNAL(clicked(bool)), this, SLOT(startSlot()));
    connect(m_stopPushButton, SIGNAL(clicked(bool)), this, SLOT(stopSlot()));
    connect(m_savePushButton, SIGNAL(clicked(bool)), this, SLOT(saveSlot()));
}

QChart *Widget::createSplineChart() const
{
    // spine chart

    mChart *chart = new mChart;
//        chart->setTitle(tr("SplineSeries"));
    chart->legend()->hide();
    chart->resize(900,500);
    chart->setAnimationOptions(QChart::NoAnimation);

    connect(this, SIGNAL(m_timerStart()), chart, SLOT(m_timerStartSlot()));
    connect(this, SIGNAL(m_timerStop()), chart, SLOT(m_timerStopSlot()));
    connect(this, SIGNAL(m_dataSave()), chart, SLOT(saveDataSlot()));
    connect(chart, SIGNAL(savep()), this, SLOT(savepn()));
    connect(chart, SIGNAL(readFinished()), this, SLOT(stopSlot()));

    return chart;
}

QLabel *Widget::createState() const
{
    QLabel *label = new QLabel();
    label->setPixmap(QPixmap(":/image/icon/off"));
//    label->setMinimumSize(67,33);

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

void Widget::themeChanged()
{
    QChart::ChartTheme theme = (QChart::ChartTheme) m_themeComboBox->itemData(m_themeComboBox->currentIndex()).toInt();

    chartView->chart()->setTheme(theme);
#if 0
    QPalette pal = window()->palette();
    if (theme == QChart::ChartThemeLight) {
        pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
    } else if (theme == QChart::ChartThemeDark) {
        pal.setColor(QPalette::Window, QRgb(0x121218));
        pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
    } else if (theme == QChart::ChartThemeBlueCerulean) {
        pal.setColor(QPalette::Window, QRgb(0x40434a));
        pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
    } else if (theme == QChart::ChartThemeBrownSand) {
        pal.setColor(QPalette::Window, QRgb(0x9e8965));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
    } else if (theme == QChart::ChartThemeBlueNcs) {
        pal.setColor(QPalette::Window, QRgb(0x018bba));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
    } else if (theme == QChart::ChartThemeHighContrast) {
        pal.setColor(QPalette::Window, QRgb(0xffab03));
        pal.setColor(QPalette::WindowText, QRgb(0x181818));
    } else if (theme == QChart::ChartThemeBlueIcy) {
        pal.setColor(QPalette::Window, QRgb(0xcee7f0));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
    } else {
        pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
    }
    window()->setPalette(pal);
#endif
}

void Widget::savepn()
{
    QPixmap pix = this->grab(QRect(chartView->geometry()));
    if (pix.save("./a.png","png"))
        qDebug() << "secces";
}

void Widget::openUartSlot()
{
    qDebug() << "uart";
    qDebug() << m_uartComboBox->count();
//    if (m_uartComboBox->count()<1)
//        return;

    if (m_on_off_uartPushButton->text() == "打开"){
        //串口设置
        m_serialPort = new QSerialPort;//创建QSerialPort对象
        m_serialPort->setPortName(m_uartComboBox->currentText());//设置串口名
        if (!m_serialPort->open(QIODevice::ReadWrite)) {//打开串口
            QMessageBox::warning(NULL, "warning", "SerialPort Open Failed");
            return;
        }
        m_serialPort->setBaudRate(QSerialPort::Baud115200);//设置串口波特率
        m_serialPort->setDataBits(QSerialPort::Data8);//设置串口数据位
        m_serialPort->setParity(QSerialPort::NoParity);//设置串口校验位
        m_serialPort->setStopBits(QSerialPort::OneStop);//设置串口停止位
        m_serialPort->setFlowControl(QSerialPort::NoFlowControl);//设置流控制
        //界面设置
        m_on_off_uartPushButton->setText(tr("关闭"));
        m_startPushButton->setEnabled(true);
    }
    else{
        m_serialPort->close();
        delete m_serialPort;
        m_serialPort = nullptr;

        m_on_off_uartPushButton->setText(tr("打开"));
        m_startPushButton->setEnabled(false);
    }
}

void Widget::writeArgumentSlot()
{
    qDebug() << "write";
//    QString str = "SET_PARAM:"+QString("%1 %2 %3 %4 %5 %6 000 000 000 000")
//            .arg(m_initatingPressureLineEdit->text().toInt(), 3, 10, QLatin1Char('0'))
//            .arg(m_stageStepUpLineEdit->text().toInt(), 3, 10, QLatin1Char('0'))
//            .arg(m_stageTimeLineEdit->text().toInt(), 3, 10, QLatin1Char('0'))
//            .arg(m_stepUpNumberLineEdit->text().toInt(), 3, 10, QLatin1Char('0'))
//            .arg(m_pressureDropLineEdit->text().toInt(), 3, 10, QLatin1Char('0'))
//            .arg(m_testPatternLineEdit->text().toInt(), 3, 10, QLatin1Char('0'));
//    m_serialPort->write(str.toLatin1().data());
}

void Widget::readArgumentSlot()
{
    qDebug() << "read";
}

void Widget::startSlot()
{
    emit m_timerStart();
    m_startPushButton->setEnabled(false);
    m_stopPushButton->setEnabled(true);
}

void Widget::stopSlot()
{
    emit m_timerStop();
    m_startPushButton->setEnabled(true);
    m_stopPushButton->setEnabled(false);
}

void Widget::saveSlot()
{
    emit m_dataSave();
}
