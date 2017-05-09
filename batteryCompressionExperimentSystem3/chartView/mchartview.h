#ifndef MCHARTVIEW_H
#define MCHARTVIEW_H

#include <QtCharts/QAbstractAxis>
#include <QtCharts/QAbstractSeries>
#include <QtCharts/QChart>
#include <QtWidgets/QGraphicsView>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QRubberBand;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE

class mChartView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit mChartView(QWidget *parent = Q_NULLPTR);
    explicit mChartView(QChart *chart, QWidget *parent = Q_NULLPTR);
    ~mChartView();

//    void setRubberBand(const RubberBands &rubberBands);
//    RubberBands rubberBand() const;

    QChart *chart() const;
    void setChart(QChart *chart);

protected:
//    void resizeEvent(QResizeEvent *event);
//    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
//    void mouseReleaseEvent(QMouseEvent *event);

private:
    QGraphicsSimpleTextItem *m_coordX;
    QGraphicsSimpleTextItem *m_coordY;
};

QT_CHARTS_END_NAMESPACE

#endif // MCHARTVIEW_H
