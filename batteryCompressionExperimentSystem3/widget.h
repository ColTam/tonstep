#ifndef WIDGET_H
#define WIDGET_H

//#define MODBUS
#define FONTSIZE 16

#include <QtWidgets/QWidget>
#include <QtCharts/QChartGlobal>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QModbusDataUnit>
#include <QThread>
#include <QTimer>

#ifdef _MSC_BUILD
#pragma execution_character_set("utf-8")
#endif

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QModbusClient;
class QModbusDataUnit;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

Q_SIGNALS:
    void m_dataSave(const QString &saveData);
    void updatePressure(const QString &pressure);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;//重绘背景图

private:
    void connectSignals(); //链接控件信号
    //界面控件初始化函数
    QLabel *createState() const;
    QLineEdit *createWriteLineEdit() const;
    QLineEdit *createReadLineEdit() const;
    QComboBox *createUartComboBox() const;
    QChart *createSplineChart() const;
    //串口通讯数据处理
    void String2Hex(const QString &str, QByteArray &senddata);
    char ConvertHexChar(char ch);
    void dealData(const QString &str);

private Q_SLOTS:
    void savepn(const QString &file);//保存图片
    void openUartSlot();//打开串口
    void writeArgumentSlot();//设置参数
    void readArgumentSlot();//读取参数

    void m_timerout();//定时器超时槽函数 用于定时读取串口数据

private:
    //界面控件
    QLabel* m_stateLabel;

    QLineEdit* m_initatingPressureLineEdit;
    QLineEdit* m_stageStepUpLineEdit;
    QLineEdit* m_stageTimeLineEdit;
    QLineEdit* m_stepUpNumberLineEdit;
    QLineEdit* m_pressureDropLineEdit;
    QLineEdit* m_testPatternLineEdit;
    QLineEdit* m_currentPressureLineEdit;
    QLineEdit* m_maxPressureLineEdit;

    QPushButton* m_writeArgumentPushButton;
    QPushButton* m_readArgumentPushButton;
    QPushButton* m_startPushButton;
    QPushButton* m_stopPushButton;
    QPushButton* m_savePushButton;
    QPushButton* m_on_off_uartPushButton;

    QComboBox* m_uartComboBox;

    QChart *m_chart;
    QChartView *m_chartView;

    //串口对象
    QSerialPort *m_serialPort;
    QString mNewData;
    QString mData;

    bool getArgument;       //需要获取参数时 为1
    bool getPressure;       //需要获取压力值时 为1
    QTimer *m_timer;

    static int mTimeoutCount;//超时计数器 作用：200ms取一次数据给曲线图
};

#endif // WIDGET_H
