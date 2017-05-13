#ifndef WIDGET_H
#define WIDGET_H

#define MODBUS
#define FONTSIZE 16

#include <QtWidgets/QWidget>
#include <QtCharts/QChartGlobal>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QModbusDataUnit>

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
    void m_timerStart();
    void m_timerStop();
    void m_dataSave(QString);

private:
    void connectSignals();
    QLabel *createState() const;
    QLineEdit *createWriteLineEdit() const;
    QLineEdit *createReadLineEdit() const;
    QComboBox *createUartComboBox() const;
    QComboBox *createThemeComboBox() const;
    QChart *createSplineChart() const;

private Q_SLOTS:
    void themeChanged();
    void savepn(QString file);
    void openUartSlot();
    void writeArgumentSlot();
    void readArgumentSlot();

    void readReady();

private:
    QLabel* m_stateLabel;

    QLineEdit* m_initatingPressureLineEdit;
    QLineEdit* m_stageStepUpLineEdit;
    QLineEdit* m_stageTimeLineEdit;
    QLineEdit* m_stepUpNumberLineEdit;
    QLineEdit* m_pressureDropLineEdit;
    QLineEdit* m_testPatternLineEdit;
    QLineEdit* m_maxPressureLineEdit;

    QPushButton* m_writeArgumentPushButton;
    QPushButton* m_readArgumentPushButton;
    QPushButton* m_startPushButton;
    QPushButton* m_stopPushButton;
    QPushButton* m_savePushButton;
    QPushButton* m_on_off_uartPushButton;

    QComboBox* m_themeComboBox;
    QComboBox* m_uartComboBox;

    QChart *m_chart;
    QChartView *m_chartView;

    QSerialPort *m_serialPort;
    QModbusClient *m_modbusClient;
    QModbusDataUnit m_setDataUnit;
    QModbusDataUnit m_getDataUnit;
    QModbusDataUnit m_readDataUnit;
};

#endif // WIDGET_H
