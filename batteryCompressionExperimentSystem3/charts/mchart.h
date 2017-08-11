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
    void writeExcel(const QString &fileName);//导出excel
    //excel数据处理
    void rangeToWrite(const QString &itemName, const uint &column, const QVariantList &varList,
                      const QString &head, QColor color = QColor(255,255,255));//整列数据写入
    void convertToColName(const int &data, QString &res);//转换为excel坐标
    QString to26AlphabetString(const int &data) const;//转换为字母

signals:
    void savep(const QString &fileName);//保存图片

public slots:
    void splineClear();//初始化曲面图界面

private Q_SLOTS:
    void saveDataSlot(const QString &file);//保存数据

    void paintPressure(const QString &pressureData);//曲线图描点处理函数

private:
    friend class Widget;

//    QLineSeries *m_splineSeries;
    QSplineSeries *m_splineSeries;//曲线
//    QGraphicsSimpleTextItem *m_coordX;
//    QGraphicsSimpleTextItem *m_coordY;

    qreal m_x;
    qreal m_y;
    qreal m_rangeY;

    QList<QPointF> dataList;//描点记录链表
    QAxObject*     _worksheet;//excel操作对象

    QVariantList mVar;
};

#endif // MCHART_H
