#ifndef MCHART_H
#define MCHART_H

#include <QtCharts/QChart>
#include <QtCore/QTimer>

QT_CHARTS_BEGIN_NAMESPACE
class QLineSeries;
class QSplineSeries;
class QValueAxis;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class mChart : public QChart
{
    Q_OBJECT
public:
    explicit mChart(QGraphicsItem *parent = 0);
    ~mChart();

signals:
    void savep(QString);
    void readFinished();

public slots:
    void handleTimeout();
    void splineClear();

private Q_SLOTS:
    void m_timerStartSlot();
    void m_timerStopSlot();
    void saveDataSlot(QString file);

    void paintPressure(QString p);

private:
    QTimer m_timer;
//    QLineSeries *m_splineSeries;
    QSplineSeries *m_splineSeries;
//    QGraphicsSimpleTextItem *m_coordX;
//    QGraphicsSimpleTextItem *m_coordY;

    qreal m_x;
    qreal m_y;
    qreal m_rangeY;

    QList<QPointF> dataList;

};

#endif // MCHART_H
