#ifndef MCHART_H
#define MCHART_H

#include <QAxObject>
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

public:
    void writeExcel(const QString &fileName);

    void rangeToWrite(const QString &itemName, const uint &column, const QVariantList &varList,
                      const QString &head, QColor color = QColor(255,255,255));
    void convertToColName(const int &data, QString &res);

    QString to26AlphabetString(const int &data) const;

signals:
    void savep(const QString &fileName);

public slots:
    void splineClear();

private Q_SLOTS:
    void saveDataSlot(const QString &file);

    void paintPressure(const QString &pressureData);

private:
    friend class Widget;

//    QLineSeries *m_splineSeries;
    QSplineSeries *m_splineSeries;
//    QGraphicsSimpleTextItem *m_coordX;
//    QGraphicsSimpleTextItem *m_coordY;

    qreal m_x;
    qreal m_y;
    qreal m_rangeY;

    QList<QPointF> dataList;
    QAxObject*     _worksheet;

    QVariantList mVar;
};

#endif // MCHART_H
