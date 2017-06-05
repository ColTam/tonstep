#include "mchart.h"
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtWidgets/QWidget>
#include <QAxObject>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QtCore/QDebug>

mChart::mChart(QGraphicsItem *parent) :
    QChart(parent),
    m_splineSeries(nullptr),
//    m_coordX(0),
//    m_coordY(0),
    m_x(0),
    m_y(0),
    m_rangeY(0),
    dataList(),
    _worksheet(nullptr)
{
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

void mChart::writeExcel(const QString &fileName)
{
    QAxObject excel("Excel.Application");
    if (excel.isNull()) excel.setControl("ET.Application");
    excel.setProperty("Visible", false);
    excel.setProperty("DisplayAlerts", false);

    QAxObject *workbooks = excel.querySubObject("WorkBooks");
    workbooks->dynamicCall("Add");
    QAxObject *workbook = excel.querySubObject("ActiveWorkBook");
    _worksheet = workbook->querySubObject("WorkSheets(int)", 1);
    QVariantList var;

    for (int i = 0; i < dataList.size(); i++)
    {
        var.append(dataList.at(i).y());
    }
    rangeToWrite("CH", 2, var, "1", QColor(255,192,0));
    var.clear();

    QAxObject *worksheets = workbook->querySubObject("Sheets");
    int sheetCount = worksheets->property("Count").toInt();    // GET WORKSHEETS COUNT
    QAxObject *lastSheet = worksheets->querySubObject("Item(int)", sheetCount);
    QAxObject *workSheet = worksheets->querySubObject("Add(QVariant)", lastSheet->asVariant());
    lastSheet->dynamicCall("Move(QVariant)", workSheet->asVariant());

    _worksheet = workbook->querySubObject("WorkSheets(int)", 2);
    QAxObject *cell = _worksheet->querySubObject("Shapes");

    QString p = fileName;
    p.replace("/", "\\");
    cell->dynamicCall("AddPicture(QString&, bool, bool, double, double, double, double",p,true,true,0,0,718,440);

    p.replace(".png", ".xlsx");
    workbook->dynamicCall("SaveAs(const QString&)", p);
    workbook->dynamicCall("Close(Boolean)", false);
    excel.dynamicCall("Quit(void)");
}

void mChart::rangeToWrite(const QString &itemName, const uint &column, const QVariantList &varList, const QString &head, QColor color)
{
    if (itemName.isEmpty() || varList.isEmpty()) return;

    QList<QList<QVariant>> m_datas;
    QVariantList port;
    port.append(itemName);
    m_datas.append(port);

    for(int i=0;i<varList.size();i++)
    {
        QList<QVariant> rows;
        rows.append(varList.at(i));
        m_datas.append(rows);
    }

    int col = column;
    int row = m_datas.size();
    QString rangStr1;
    QString rangStr2;
    convertToColName(col,rangStr1);
    rangStr1.replace("@", "Z");
    rangStr2 = rangStr1;
    rangStr1 += head + ":" + rangStr1 + QString::number(row+head.toInt()-1);
    QAxObject* range = _worksheet->querySubObject("Range(const QString&)",rangStr1);
    if (NULL == range || range->isNull()) return;

    QVariantList vars;
    for (int i=0;i<row;i++)
        vars.append(QVariant(m_datas[i]));

    range->setProperty("Value", vars);
    range->setProperty("HorizontalAlignment", -4108);

    QAxObject *Borders = range->querySubObject("Borders");//Interior background; Borders border; Font font;
    Borders->setProperty("Color", color);
}

void mChart::convertToColName(const int &data, QString &res)
{
    int tempData = data / 26;

    if (1 == tempData && 26 == data){
        res = to26AlphabetString(data);
        return;
    } else if (tempData > 0) {
        int mode = data % 26;
        if (0 == mode) {
            --tempData;
            mode = 26;
        }
        convertToColName(mode,res);
        convertToColName(tempData,res);
    } else {
        res = (to26AlphabetString(data) + res);
    }
}

QString mChart::to26AlphabetString(const int &data) const
{
    QChar ch = data + 0x40;//A:0x41
    return QString(ch);
}

void mChart::saveDataSlot(const QString &file)
{
    this->axisX()->setRange(0, m_x);
    for (int i = 0; i < dataList.size(); i++)
        qDebug() << dataList.at(i).y();

    QList<QPointF>::iterator i = dataList.begin();
    while (i++!=dataList.end())
        qDebug() << *i;

    qDebug() << file;
    emit savep(file);
    writeExcel(file);
}

void mChart::paintPressure(const QString &pressureData)
{
    qDebug() << QTime::currentTime().toString("hh-mm-ss-zzz");
    qreal x = plotArea().width()/(25.0/2);//p.w/x.c * m_x
    m_x+= 2;
    m_y = pressureData.toInt(nullptr, 16);
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
}

void mChart::splineClear()
{
    m_splineSeries->clear();
    m_rangeY = 0;
    m_x = 0;
    m_y = 0;

    this->axisX()->setRange(-5, 20);
    this->axisY()->setRange(0, 200);
}
