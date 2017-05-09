#ifndef WIDGET_H
#define WIDGET_H

#define FONTSIZE 16

#include <QtWidgets/QWidget>
#include <QtCharts/QChartGlobal>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#ifdef _MSC_BUILD
#pragma execution_character_set("utf-8")
#endif

class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;

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
    void m_dataSave();

private:
    void connectSignals();
    QLabel *createState() const;
    QLabel *createLabel(QString name) const;
    QLineEdit *createWriteLineEdit() const;
    QLineEdit *createReadLineEdit() const;
    QPushButton *createPushButton(QString name) const;
    QComboBox *createUartComboBox() const;
    QComboBox *createThemeComboBox() const;
    QChart *createSplineChart() const;

private Q_SLOTS:
    void themeChanged();
    void savepn();
    void openUartSlot();
    void writeArgumentSlot();
    void readArgumentSlot();
    void startSlot();
    void stopSlot();
    void saveSlot();

private:
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

    QChart *c;
    QChartView *chartView;

    QSerialPort *m_serialPort;
};

#endif // WIDGET_H
