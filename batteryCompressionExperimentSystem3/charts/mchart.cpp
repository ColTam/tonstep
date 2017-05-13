#include "mchart.h"
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtWidgets/QWidget>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QtCore/QDebug>

mChart::mChart(QGraphicsItem *parent) :
    QChart(parent),
    m_splineSeries(0),
//    m_coordX(0),
//    m_coordY(0),
    m_x(0),
    m_y(0),
    m_rangeY(0)
{
    qsrand((uint) QTime::currentTime().msec());

    QObject::connect(&m_timer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
    m_timer.setInterval(100);

//    m_splineSeries = new QLineSeries(this);
    m_splineSeries = new QSplineSeries(this);
    m_splineSeries->append(m_x, m_y);

    QValueAxis *xAxis = new QValueAxis();
    xAxis->setRange(-5, 20);
    xAxis->setTickCount(6);
    xAxis->setMinorTickCount(4);
    xAxis->setLabelFormat("%d");
    xAxis->setTitleText(tr("Time (0.1s)"));
//    xAxis->setLabelsVisible(false);

    QValueAxis *yAxis = new QValueAxis();
    yAxis->setRange(0, 200);
    yAxis->setTickCount(11);
    yAxis->setMinorTickCount(1);
    yAxis->setLabelFormat("%d");
    yAxis->setTitleText(tr("Pressure Value (1PSL)"));

    addSeries(m_splineSeries);
    createDefaultAxes();
//    setAcceptHoverEvents(true);

    setAxisX(xAxis, m_splineSeries);
    setAxisY(yAxis, m_splineSeries);


//    m_coordX = new QGraphicsSimpleTextItem(this);
//    m_coordX->setPos(this->size().width()/2 +20, 470);
//    m_coordX->setText("X: ");
//    m_coordY = new QGraphicsSimpleTextItem(this);
//    m_coordY->setPos(this->size().width()/2 +60, 470);
//    m_coordY->setText("Y: ");
}

mChart::~mChart()
{

}

void mChart::handleTimeout()
{
    qDebug() << QTime::currentTime().toString("hh-mm-ss-zzz");
    qreal x = plotArea().width()/25;
    m_x+= 1;
    m_y = qrand() % 300 + 50;
    m_splineSeries->append(m_x, m_y);

    if (m_x > 19) {
        scroll(x, 0);
    }

    if (m_rangeY < m_y) {
        m_rangeY = m_y;
    }
    this->axisY()->setRange(0, m_rangeY + 20);

    dataList.clear();
    dataList = m_splineSeries->points();

    if (m_x >= 600) {
        m_timer.stop();
        emit readFinished();
    }
}

void mChart::m_timerStartSlot()
{
    if (m_x >= 600) {
        m_splineSeries->clear();
        m_rangeY = 0;
        m_x = 0;
        m_y = 0;

        this->axisX()->setRange(-5, 25);
        this->axisY()->setRange(0, 200);
    } else if (m_x >= 19 && m_x < 600) {
        this->axisX()->setRange(m_x-24, m_x+1);
    } else {
        this->axisX()->setRange(-5, 25);
    }

    m_timer.start(100);
}

void mChart::m_timerStopSlot()
{
    m_timer.stop();
}

void mChart::saveDataSlot(QString file)
{
    this->axisX()->setRange(0, m_x);
    for (int i = 0; i < dataList.size(); i++)
        qDebug() << dataList.at(i).y();

    emit savep(file);
}
