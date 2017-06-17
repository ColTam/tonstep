﻿#include "widget.h"
#include "Collect.h"
#include "titlebar.h"
#include "formdisplay.h"
#include "temperaturerisedialog.h"
#include "lifetesterdialog.h"
#include "qmessagewidget.h"

#include <QFile>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QTextStream>
#include <QDesktopWidget>
#include <QTimer>
#include <QApplication>
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent)
  ,_collect(new Collect)
  ,_titleBar(new TitleBar(this))
  ,_formDisplay(new FormDisplay(this))
  ,_animation(new QPropertyAnimation(this, "windowOpacity"))
  ,_testThread(new QThread())
  ,_mTestThread(new mTestThread())
  ,_tRCRThread(new QThread())
  ,_mTRCRThread(new mTRCRThread())
  ,_isTR(false)
  ,_isIngA(false)
  ,_isIngB(false)
  ,_isIngC(false)
{
    //init STAS
    connectSTAS = _collect->ConnectToSTAS();
    Collect::CutToLN("A");
    Collect::CutToLN("B");
    Collect::CutToLN("C");
    Collect::AlarmClose();
    Collect::TestDisplayOpen();

    //init widget
    initWidget();

    _mTestThread->moveToThread(_testThread);
    connect(_testThread, SIGNAL(started()), _mTestThread, SLOT(readData()));
    connect(_mTestThread, SIGNAL(testDataA(QString)), this, SIGNAL(testDataA(QString)), Qt::DirectConnection);
    connect(_mTestThread, SIGNAL(testDataB(QString)), this, SIGNAL(testDataB(QString)), Qt::DirectConnection);
    connect(_mTestThread, SIGNAL(testDataC(QString)), this, SIGNAL(testDataC(QString)), Qt::DirectConnection);
    connect(this, SIGNAL(testTimerStart()), _mTestThread, SLOT(mTestTimerStart()));
    connect(this, SIGNAL(testTimerStop()), _mTestThread, SLOT(mTestTimerStop()));
    _testThread->start();

    _mTRCRThread->moveToThread(_tRCRThread);
    connect(_tRCRThread, SIGNAL(started()), _mTRCRThread, SLOT(readData()));
    connect(_mTRCRThread, SIGNAL(tRCRData(QStringList)), this, SIGNAL(tRCRData(QStringList)), Qt::DirectConnection);
    connect(this, SIGNAL(tRCRTimerStart()), _mTRCRThread, SLOT(mTRCRTimerStart()));
    connect(this, SIGNAL(tRCRTimerStop()), _mTRCRThread, SLOT(mTRCRTimerStop()));
    _tRCRThread->start();
}

Widget::~Widget()
{
    _mTRCRThread = NULL;
    delete _mTRCRThread;
    _tRCRThread = NULL;
    delete _tRCRThread;
    _mTestThread = NULL;
    delete _mTestThread;
    _testThread = NULL;
    delete _testThread;

    _collect = NULL;
    delete _collect;
}

void Widget::initWidgetTitle()
{
    this->setWindowFlags(Qt::FramelessWindowHint);

    this->installEventFilter(_titleBar);
    this->resize(WINDOW_WIDTH, WINDOW_HEIGHT);
    this->setWindowTitle(tr(" Electrical Accessories Test Automation Program"));
    this->setWindowIcon(QIcon(WINDOW_ICON));

//    QPalette mPalette(palette());
//    mPalette.setColor(QPalette::Background, QColor(64,66,68));
//    this->setAutoFillBackground(true);
//    this->setPalette(mPalette);
//    this->setStyleSheet("QWidget{"
//                        "background-color: qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,"
//                        "stop:0 #484848,stop:1 #383838);"
//                        "}");

    QVBoxLayout *titleVBoxLayout = new QVBoxLayout();
    titleVBoxLayout->addWidget(_titleBar);
    titleVBoxLayout->addStretch(0);
    titleVBoxLayout->setSpacing(0);
    titleVBoxLayout->setContentsMargins(0, 0, 0, 0);

    connect(_titleBar, SIGNAL(closeClicked()), this, SLOT(closeWidget()));

    this->setLayout(titleVBoxLayout);
}

void Widget::initWidget()
{
    initWidgetTitle();

    _formDisplay->setGeometry(0, 26, 960, 694);

    connect(_formDisplay, SIGNAL(isSectionFileName(QString,int)), this, SLOT(selectClause(QString,int)));
    connect(this, SIGNAL(finishedItem(QString, int)), _formDisplay, SLOT(finishedItem(QString, int)));
    connect(_formDisplay, SIGNAL(tTimerStop(int,int)), this, SLOT(tTimerStop(int,int)));
}

void Widget::newTemperatureDialog(time_t hours, QString fileName, QStringList tc_list, int roomT2, QString servo, int num, QString Im)
{
    if (servo == "A") {
        TemperatureRiseDialog *temperatureRiseA = new TemperatureRiseDialog(hours, num, Im, NULL);
        temperatureRiseA->move((QApplication::desktop()->width() - temperatureRiseA->width())/2,
                                   (QApplication::desktop()->height() - temperatureRiseA->height())/2);
        temperatureRiseA->setWindowTitle(tr(" Electrical Accessories Test Automation Program-")+fileName);

        connect(this, SIGNAL(temperatureRiseUpperA(QStringList,int)), temperatureRiseA, SLOT(temprtatureRiseUpper(QStringList,int)));
        connect(this, SIGNAL(tRCRData(QStringList)), temperatureRiseA, SLOT(updateTC(QStringList)));
        connect(temperatureRiseA, SIGNAL(clauseFinishedA(int)), this, SLOT(finishedClauseA(int)));

        emit temperatureRiseUpperA(tc_list, roomT2);
        temperatureRiseA->show();
    } else if (servo == "B") {
        TemperatureRiseDialog *temperatureRiseB = new TemperatureRiseDialog(hours, num, Im, NULL);
        temperatureRiseB->move((QApplication::desktop()->width() - temperatureRiseB->width())/2,
                                   (QApplication::desktop()->height() - temperatureRiseB->height())/2);
        temperatureRiseB->setWindowTitle(tr(" Electrical Accessories Test Automation Program-")+fileName);

        connect(this, SIGNAL(temperatureRiseUpperB(QStringList,int)), temperatureRiseB, SLOT(temprtatureRiseUpper(QStringList,int)));
        connect(this, SIGNAL(tRCRData(QStringList)), temperatureRiseB, SLOT(updateTC(QStringList)));
        connect(temperatureRiseB, SIGNAL(clauseFinishedB(int)), this, SLOT(finishedClauseB(int)));

        emit temperatureRiseUpperB(tc_list, roomT2);
        temperatureRiseB->show();
    } else if (servo == "C") {
        TemperatureRiseDialog *temperatureRiseC = new TemperatureRiseDialog(hours, num, Im, NULL);
        temperatureRiseC->move((QApplication::desktop()->width() - temperatureRiseC->width())/2,
                                   (QApplication::desktop()->height() - temperatureRiseC->height())/2);
        temperatureRiseC->setWindowTitle(tr(" Electrical Accessories Test Automation Program-")+fileName);

        connect(this, SIGNAL(temperatureRiseUpperC(QStringList,int)), temperatureRiseC, SLOT(temprtatureRiseUpper(QStringList,int)));
        connect(this, SIGNAL(tRCRData(QStringList)), temperatureRiseC, SLOT(updateTC(QStringList)));
        connect(temperatureRiseC, SIGNAL(clauseFinishedC(int)), this, SLOT(finishedClauseC(int)));

        emit temperatureRiseUpperC(tc_list, roomT2);
        temperatureRiseC->show();
    }
}

void Widget::newLifeTesterDialog(QString fileName, QString servo)
{
#if 1
    if (servo == "A") {
        LifeTesterDialog *lifeTesterDialogA = new LifeTesterDialog(NULL);
        lifeTesterDialogA->move((QApplication::desktop()->width() - lifeTesterDialogA->width())/2,
                            (QApplication::desktop()->height() - lifeTesterDialogA->height())/2);
        lifeTesterDialogA->setWindowTitle(tr(" Electrical Accessories Test Automation Program-")+fileName);

        connect(lifeTesterDialogA, SIGNAL(clauseFinishedA(int)), this, SLOT(finishedClauseA(int)));
        connect(lifeTesterDialogA, SIGNAL(testTimerStart()), this, SLOT(testStartRead()));
        connect(lifeTesterDialogA, SIGNAL(testTimerStop()), this, SLOT(testStopRead()));
        connect(lifeTesterDialogA, SIGNAL(tRcrTimerStart()), this, SLOT(tTimerStart()));
        connect(lifeTesterDialogA, SIGNAL(tRcrTimerStop()), this, SIGNAL(tRCRTimerStop()));
        connect(lifeTesterDialogA, SIGNAL(errorStart(QString)), this, SLOT(testError(QString)));
        connect(lifeTesterDialogA, SIGNAL(errorStop(QString)), this, SLOT(testNoError(QString)));
        connect(this, SIGNAL(testDataA(QString)), lifeTesterDialogA, SLOT(updateTest(QString)), Qt::DirectConnection);
        connect(this, SIGNAL(testAThreadStart()), lifeTesterDialogA, SLOT(errorLoadStart()));
        connect(this, SIGNAL(testAThreadStop()), lifeTesterDialogA, SLOT(errorLoadStop()));

        lifeTesterDialogA->show();
    } else if (servo == "B") {
        LifeTesterDialog *lifeTesterDialogB = new LifeTesterDialog(NULL);
        lifeTesterDialogB->move((QApplication::desktop()->width() - lifeTesterDialogB->width())/2,
                            (QApplication::desktop()->height() - lifeTesterDialogB->height())/2);
        lifeTesterDialogB->setWindowTitle(tr(" Electrical Accessories Test Automation Program-")+fileName);

        connect(lifeTesterDialogB, SIGNAL(clauseFinishedB(int)), this, SLOT(finishedClauseB(int)));
        connect(lifeTesterDialogB, SIGNAL(testTimerStart()), this, SLOT(testStartRead()));
        connect(lifeTesterDialogB, SIGNAL(testTimerStop()), this, SLOT(testStopRead()));
        connect(lifeTesterDialogB, SIGNAL(tRcrTimerStart()), this, SLOT(tTimerStart()));
        connect(lifeTesterDialogB, SIGNAL(tRcrTimerStop()), this, SIGNAL(tRCRTimerStop()));
        connect(lifeTesterDialogB, SIGNAL(errorStart(QString)), this, SLOT(testError(QString)));
        connect(lifeTesterDialogB, SIGNAL(errorStop(QString)), this, SLOT(testNoError(QString)));
        connect(this, SIGNAL(testDataB(QString)), lifeTesterDialogB, SLOT(updateTest(QString)), Qt::DirectConnection);
        connect(this, SIGNAL(testBThreadStart()), lifeTesterDialogB, SLOT(errorLoadStart()));
        connect(this, SIGNAL(testBThreadStop()), lifeTesterDialogB, SLOT(errorLoadStop()));

        lifeTesterDialogB->show();
    } else if (servo == "C") {
        LifeTesterDialog *lifeTesterDialogC = new LifeTesterDialog(NULL);
        lifeTesterDialogC->move((QApplication::desktop()->width() - lifeTesterDialogC->width())/2,
                            (QApplication::desktop()->height() - lifeTesterDialogC->height())/2);
        lifeTesterDialogC->setWindowTitle(tr(" Electrical Accessories Test Automation Program-")+fileName);

        connect(lifeTesterDialogC, SIGNAL(clauseFinishedC(int)), this, SLOT(finishedClauseC(int)));
        connect(lifeTesterDialogC, SIGNAL(testTimerStart()), this, SLOT(testStartRead()));
        connect(lifeTesterDialogC, SIGNAL(testTimerStop()), this, SLOT(testStopRead()));
        connect(lifeTesterDialogC, SIGNAL(tRcrTimerStart()), this, SLOT(tTimerStart()));
        connect(lifeTesterDialogC, SIGNAL(tRcrTimerStop()), this, SIGNAL(tRCRTimerStop()));
        connect(lifeTesterDialogC, SIGNAL(errorStart(QString)), this, SLOT(testError(QString)));
        connect(lifeTesterDialogC, SIGNAL(errorStop(QString)), this, SLOT(testNoError(QString)));
        connect(this, SIGNAL(testDataC(QString)), lifeTesterDialogC, SLOT(updateTest(QString)), Qt::DirectConnection);
        connect(this, SIGNAL(testCThreadStart()), lifeTesterDialogC, SLOT(errorLoadStart()));
        connect(this, SIGNAL(testCThreadStop()), lifeTesterDialogC, SLOT(errorLoadStop()));

        lifeTesterDialogC->show();
    }
#else
//    LifeTesterDialog *lifeTesterDialog = new LifeTesterDialog(NULL);
//    lifeTesterDialog->move((QApplication::desktop()->width() - lifeTesterDialog->width())/2,
//                        (QApplication::desktop()->height() - lifeTesterDialog->height())/2);
//    lifeTesterDialog->setWindowTitle(tr(" Electrical Accessories Test Automation Program-")+fileName);

//    connect(lifeTesterDialog, SIGNAL(clauseFinishedA(int)), this, SLOT(finishedClauseA(int)));
//    connect(lifeTesterDialog, SIGNAL(testTimerStart()), this, SLOT(testStartRead()));
//    connect(lifeTesterDialog, SIGNAL(testTimerStop()), this, SLOT(testStopRead()));
//    connect(lifeTesterDialog, SIGNAL(tRcrTimerStart()), this, SLOT(tTimerStart()));
//    connect(lifeTesterDialog, SIGNAL(tRcrTimerStop()), this, SIGNAL(tRCRTimerStop()));
//    connect(this, SIGNAL(testDataA(QString)), lifeTesterDialog, SLOT(updateTest(QString)), Qt::DirectConnection);

//    lifeTesterDialog->show();
#endif
}

void Widget::closeWidget()
{
    _formDisplay->close();
    _tRCRThread->quit();
    _mTRCRThread->deleteLater();
    _tRCRThread->deleteLater();
    _testThread->quit();
    _mTestThread->deleteLater();
    _testThread->deleteLater();

    //widget close effect
    _animation->setDuration(999); //time /ms
    _animation->setStartValue(1);
    _animation->setEndValue(0);
    _animation->start();

    connect(_animation, SIGNAL(finished()), this, SLOT(close()));
}

void Widget::selectClause(QString fileName, int iec)
{
    mServo = fileName.right(2).mid(0, 1);
    if (mServo == "A") {
        mFileNameA = fileName;
    } else if (mServo == "B") {
        mFileNameB = fileName;
    } else if (mServo == "C") {
        mFileNameC = fileName;
    }

    if (iec) {
        switch (fileName.right(5).mid(0, 2).toInt()) {
        case CLAUSE_19:
        {
            temperatureRise(fileName);
            _isTR = true;
            break;
        }
        case CLAUSE_20:
        {
            breakingCapacityTest(fileName);
            break;
        }
        case CLAUSE_21:
        {
            normalOperation(fileName);
            break;
        }
        case CLAUSE_22:
        {
            normalOperationTemperatureRise(fileName);
            _isTR = true;
            break;
        }
        }
    } else {
        switch (fileName.right(5).mid(0, 2).toInt()) {
        case CLAUSE_19:
        {
            linkerTemperature(fileName);
            _isTR = true;
            break;
        }
        case CLAUSE_20:
        {
            breakingCapacityTest(fileName, iec);
            break;
        }
        case CLAUSE_21:
        {
            linkerNormalOperation(fileName);
            break;
        }
        }
    }
    Collect::stateRunning();
}

void Widget::tTimerStart()
{
    if (_isTR) {
        emit tRCRTimerStart();
    }
}

void Widget::tTimerStop(int r, int e)
{
    if (r == 0) {
        emit tRCRTimerStop();
        _isTR = false;
    }

    if (e == 0) {
        emit testTimerStop();
    }
}

void Widget::testStartRead()
{
    emit testTimerStart();
}

void Widget::testStopRead()
{
    emit testTimerStop();
}

void Widget::testError(QString servo)
{
    if (Collect::IsTestServoA) {
        _isIngA = true;
        if (servo != "A") {
            emit testAThreadStop();
            Collect::TestStop("A");
        }
    }
    if (Collect::IsTestServoB) {
        _isIngB = true;
        if (servo != "B") {
            emit testBThreadStop();
            Collect::TestStop("B");
        }
    }
    if (Collect::IsTestServoC) {
        _isIngC = true;
        if (servo != "C") {
            emit testCThreadStop();
            Collect::TestStop("C");
        }
    }
}

void Widget::testNoError(QString servo)
{
    if (_isIngA && servo != "A") {
        Collect::TestStart("A");
        emit testAThreadStart();
    }
    if (_isIngB && servo != "B") {
        Collect::TestStart("B");
        emit testBThreadStart();
    }
    if (_isIngC && servo != "C") {
        Collect::TestStart("C");
        emit testCThreadStart();
    }
    _isIngA = false;
    _isIngB = false;
    _isIngC = false;
}
//clause 19 / temperature rise
void Widget::temperatureRise(QString fileName)
{
    QFile itemFile(fileName);
    itemFile.open(QFile::ReadOnly | QIODevice::Text);
    if (itemFile.isOpen()) {
        QTextStream in(&itemFile);
        in.readLine();//specific
        in.readLine();//Vn
        in.readLine();//In
        QString Hours = in.readLine();//H
        bool LN_LE_cut = in.readLine().toInt();//sw
        bool IC = in.readLine().toInt();//ic
        QString Im = in.readLine();//Im
        in.readLine();//type
        in.readLine();//area
        QString Vn = in.readLine();//Vn_t
        QString In = in.readLine();//In_t
        int rT2 = in.readLine().toInt();
        QStringList list;//TR
        for (int i = 0; i<20; i++)
            list << in.readLine();
        itemFile.close();

        if (_mTRCRThread->mtRCRIsActive()) {
            emit tRCRTimerStop();
        }
        _collect->SetLoad(SET_LPF_RESISTIVE_884, In, mServo);
        _collect->LoadStart(mServo);
        _collect->SetVolt(Vn);
        _collect->PowerStart();

        if (mServo == "A")
            recordValue(2, SET_LPF_RESISTIVE_884, In, "", "", "", "");
        else if (mServo == "B")
            recordValue(3, SET_LPF_RESISTIVE_884, In, "", "", "", "");
        else if (mServo == "C")
            recordValue(4, SET_LPF_RESISTIVE_884, In, "", "", "", "");

        if (Im.toInt() == 0 && !IC) {
            if (LN_LE_cut) {
                newTemperatureDialog(Hours.toInt(), fileName, list, rT2, mServo, 2);
            } else {
                newTemperatureDialog(Hours.toInt(), fileName, list, rT2, mServo, 1);
            }
        } else if (Im.toInt() != 0 && IC) {
            _collect->ComponentsShortCut(mServo);
            if (LN_LE_cut) {
                newTemperatureDialog(Hours.toInt(), fileName, list, rT2, mServo, 4, Im);
            } else {
                newTemperatureDialog(Hours.toInt(), fileName, list, rT2, mServo, 2, Im);
            }
        }
    }

    if (!_mTRCRThread->mtRCRIsActive()) {
        emit tRCRTimerStart();
    }

    //cut to L-N
    //??
}

//clause 20 / break capacity
void Widget::breakingCapacityTest(QString fileName, int iec)
{
    QFile itemFile(fileName);
    itemFile.open(QFile::ReadOnly | QIODevice::Text);
    if (itemFile.isOpen()) {
        QTextStream in(&itemFile);
        in.readLine();
        in.readLine();
        in.readLine();
//        QString Vn = CLAUSE_20_VN(in.readLine());
//        QString In = CLAUSE_20_IN(in.readLine());
        QString Rate = in.readLine();
        QString Position = in.readLine();
        QString Vn = in.readLine();//Vn_t
        QString In = in.readLine();//In_t
        QString Nu = in.readLine();
        QString Time = in.readLine();//??/min
        in.readLine();//s
        itemFile.close();

        if (_mTestThread->mTestIsActive()) {
            emit testTimerStop();
        }
        _collect->TestClearNumber(mServo);
        if (iec) {
            _collect->SetLoad(SET_LPF_INDUCTIVE_0_6, In, mServo);
            if (mServo == "A")
                recordValue(5, SET_LPF_INDUCTIVE_0_6, In, Nu, Rate, Position, Time);
            else if (mServo == "B")
                recordValue(6, SET_LPF_INDUCTIVE_0_6, In, Nu, Rate, Position, Time);
            else if (mServo == "C")
                recordValue(7, SET_LPF_INDUCTIVE_0_6, In, Nu, Rate, Position, Time);
        } else {
            if (In.toDouble() <= 0.25) {
                return;
            } else if (In.toDouble() > 0.25 && In.toDouble() < 12.5) {
                _collect->SetLoad(SET_LPF_INDUCTIVE_0_6, In, mServo);
                if (mServo == "A")
                    recordValue(5, SET_LPF_INDUCTIVE_0_6, In, Nu, Rate, Position, Time);
                else if (mServo == "B")
                    recordValue(6, SET_LPF_INDUCTIVE_0_6, In, Nu, Rate, Position, Time);
                else if (mServo == "C")
                    recordValue(7, SET_LPF_INDUCTIVE_0_6, In, Nu, Rate, Position, Time);
            } else {
                _collect->SetLoad(SET_LPF_RESISTIVE_320, In, mServo);
                if (mServo == "A")
                    recordValue(5, SET_LPF_RESISTIVE_320, In, Nu, Rate, Position, Time);
                else if (mServo == "B")
                    recordValue(6, SET_LPF_RESISTIVE_320, In, Nu, Rate, Position, Time);
                else if (mServo == "C")
                    recordValue(7, SET_LPF_RESISTIVE_320, In, Nu, Rate, Position, Time);
            }
        }
        _collect->LoadStart(mServo);
        _collect->SetTest(CLAUSE_20_HANDLENUMBER, Rate, Position, Time, mServo);
        _collect->TestComeBackToOrigin(mServo);
        _collect->SetVolt(Vn);
        _collect->PowerStart();

        newLifeTesterDialog(fileName, mServo);
    }
}

//clause 21 normal operation
void Widget::normalOperation(QString fileName)
{
    QFile itemFile(fileName);
    itemFile.open(QFile::ReadOnly | QIODevice::Text);
    if (itemFile.isOpen()) {
        QTextStream in(&itemFile);
        in.readLine();
        in.readLine();
        in.readLine();
        QString Rate = in.readLine();
        QString Position = in.readLine();
        in.readLine();
        in.readLine();
        QString Vn = in.readLine();
        QString In = in.readLine();
        QString Nu = in.readLine();
        QString Time = in.readLine();
        in.readLine();//s
        itemFile.close();

        if (_mTestThread->mTestIsActive()) {
            emit testTimerStop();
        }
        _collect->TestClearNumber(mServo);

        _collect->SetLoad(SET_LPF_INDUCTIVE_0_8, In, mServo);
        if (Nu == "10000")
            _collect->SetTest(CLAUSE_21_HANDLENUMBER_884_FIRST, Rate, Position, Time, mServo);//共5000次  先进行2250 再进行剩下操作
        else
            _collect->SetTest(Nu, Rate, Position, Time, mServo);
        _collect->TestComeBackToOrigin(mServo);
        _collect->LoadStart(mServo);
        _collect->SetVolt(Vn);
        _collect->PowerStart();

        if (mServo == "A")
            if (Nu == "10000") {
                recordValue(5, SET_LPF_INDUCTIVE_0_8, In, CLAUSE_21_HANDLENUMBER_884_FIRST, Rate, Position, Time);
            } else {
                recordValue(5, SET_LPF_INDUCTIVE_0_8, In, Nu, Rate, Position, Time);
            }
        else if (mServo == "B")
            if (Nu == "10000") {
                recordValue(6, SET_LPF_INDUCTIVE_0_8, In, CLAUSE_21_HANDLENUMBER_884_FIRST, Rate, Position, Time);
            } else {
                recordValue(5, SET_LPF_INDUCTIVE_0_8, In, Nu, Rate, Position, Time);
            }
        else if (mServo == "C")
            if (Nu == "10000") {
                recordValue(7, SET_LPF_INDUCTIVE_0_8, In, CLAUSE_21_HANDLENUMBER_884_FIRST, Rate, Position, Time);
            } else {
                recordValue(5, SET_LPF_INDUCTIVE_0_8, In, Nu, Rate, Position, Time);
            }

        newLifeTesterDialog(fileName, mServo);
    }
}

//clause 21.1 / normaloperation temperature rise
void Widget::normalOperationTemperatureRise(QString fileName)
{
    QFile itemFile(fileName);
    itemFile.open(QFile::ReadOnly | QIODevice::Text);
    if (itemFile.isOpen()) {
        QTextStream in(&itemFile);
        in.readLine();
        in.readLine();
        in.readLine();
        QString Hours = in.readLine();
        bool LN_LE_cut = in.readLine().toInt();//sw
        in.readLine();
        in.readLine();
        QString Vn = in.readLine();
        QString In = in.readLine();
        int rT2 = in.readLine().toInt();
        QStringList list;
        for (int i = 0; i<20; i++)
            list << in.readLine();
        itemFile.close();

        if (_mTRCRThread->mtRCRIsActive()) {
            emit tRCRTimerStop();
        }
        _collect->SetLoad(SET_LPF_RESISTIVE_884, In, mServo);
        _collect->LoadStart(mServo);
        _collect->SetVolt(Vn);
        _collect->PowerStart();

        if (mServo == "A")
            recordValue(2, SET_LPF_RESISTIVE_884, In, "", "", "", "");
        else if (mServo == "B")
            recordValue(3, SET_LPF_RESISTIVE_884, In, "", "", "", "");
        else if (mServo == "C")
            recordValue(4, SET_LPF_RESISTIVE_884, In, "", "", "", "");

        if (LN_LE_cut) {
            newTemperatureDialog(Hours.toInt(), fileName, list, rT2, mServo, 2);
        } else {
            newTemperatureDialog(Hours.toInt(), fileName, list, rT2, mServo, 1);
        }

        if (!_mTRCRThread->mtRCRIsActive()) {
            emit tRCRTimerStart();
        }
    }
}

//clause 22 / temperature
void Widget::linkerTemperature(QString fileName)
{
    QFile itemFile(fileName);
    itemFile.open(QFile::ReadOnly | QIODevice::Text);
    if (itemFile.isOpen()) {
        QTextStream in(&itemFile);
        in.readLine();
        in.readLine();
        in.readLine();
        QString Hours = in.readLine();
        QString Vn = in.readLine();
        QString In = in.readLine();
        int rT2 = in.readLine().toInt();
        QStringList list;
        for (int i = 0; i<20; i++)
            list << in.readLine();
        itemFile.close();

        if (In.toDouble() > 0.2) {
            if (_mTRCRThread->mtRCRIsActive()) {
                emit tRCRTimerStop();
            }
            _collect->SetLoad(SET_LPF_RESISTIVE_884, In, mServo);
            _collect->LoadStart(mServo);
            _collect->SetVolt(Vn);
            _collect->PowerStart();

            if (mServo == "A")
                recordValue(2, SET_LPF_RESISTIVE_884, In, "", "", "", "");
            else if (mServo == "B")
                recordValue(3, SET_LPF_RESISTIVE_884, In, "", "", "", "");
            else if (mServo == "C")
                recordValue(4, SET_LPF_RESISTIVE_884, In, "", "", "", "");

            newTemperatureDialog(Hours.toInt(), fileName, list, rT2, mServo, 1);
            if (!_mTRCRThread->mtRCRIsActive()) {
                emit tRCRTimerStart();
            }
        }
    }
}

//clause 21 / normal operation
void Widget::linkerNormalOperation(QString fileName)
{
    QFile itemFile(fileName);
    itemFile.open(QFile::ReadOnly | QIODevice::Text);
    if (itemFile.isOpen()) {
        QTextStream in(&itemFile);
        in.readLine();
        in.readLine();
        in.readLine();
        QString Rate = in.readLine();
        QString Position = in.readLine();
        QString Vn = in.readLine();
        QString In = in.readLine();
        QString Nu = in.readLine();
        in.readLine();
        itemFile.close();

        if (_mTestThread->mTestIsActive()) {
            emit testTimerStop();
        }
        _collect->TestClearNumber(mServo);

        if (0 < In.toInt() && In.toDouble() <= 0.2) {
            _collect->SetTest(Nu, Rate, Position, "30", mServo);
            _collect->TestStart(mServo);

            newLifeTesterDialog(fileName, mServo);
        } else if (0.2 < In.toDouble()) {
            if (In.toDouble() < 10) {
                _collect->SetLoad(SET_LPF_INDUCTIVE_0_6, In, mServo);
                if (mServo == "A") {
                    if (Nu == "8000")
                        recordValue(5, SET_LPF_INDUCTIVE_0_6, In, CLAUSE_21_HANDLENUMBER_320_ONLOAD, Rate, Position, "30");
                    else {
                        recordValue(5, SET_LPF_INDUCTIVE_0_6, In, Nu, Rate, Position, "30");
                    }
                } else if (mServo == "B") {
                    if (Nu == "8000")
                        recordValue(6, SET_LPF_INDUCTIVE_0_6, In, CLAUSE_21_HANDLENUMBER_320_ONLOAD, Rate, Position, "30");
                    else {
                        recordValue(6, SET_LPF_INDUCTIVE_0_6, In, Nu, Rate, Position, "30");
                    }
                } else if (mServo == "C") {
                    if (Nu == "8000")
                        recordValue(7, SET_LPF_INDUCTIVE_0_6, In, CLAUSE_21_HANDLENUMBER_320_ONLOAD, Rate, Position, "30");
                    else {
                        recordValue(7, SET_LPF_INDUCTIVE_0_6, In, Nu, Rate, Position, "30");
                    }
                }
            } else {
                _collect->SetLoad(SET_LPF_RESISTIVE_884, In, mServo);
                if (mServo == "A") {
                    if (Nu == "8000")
                        recordValue(5, SET_LPF_RESISTIVE_884, In, CLAUSE_21_HANDLENUMBER_320_ONLOAD, Rate, Position, "30");
                    else {
                        recordValue(5, SET_LPF_RESISTIVE_884, In, Nu, Rate, Position, "30");
                    }
                } else if (mServo == "B") {
                    if (Nu == "8000")
                        recordValue(6, SET_LPF_RESISTIVE_884, In, CLAUSE_21_HANDLENUMBER_320_ONLOAD, Rate, Position, "30");
                    else {
                        recordValue(6, SET_LPF_RESISTIVE_884, In, Nu, Rate, Position, "30");
                    }
                } else if (mServo == "C") {
                    if (Nu == "8000")
                        recordValue(7, SET_LPF_RESISTIVE_884, In, CLAUSE_21_HANDLENUMBER_320_ONLOAD, Rate, Position, "30");
                    else {
                        recordValue(7, SET_LPF_RESISTIVE_884, In, Nu, Rate, Position, "30");
                    }
                }
            }
            _collect->LoadStart(mServo);
            if (Nu == "8000") {
                _collect->SetTest(CLAUSE_21_HANDLENUMBER_320_ONLOAD, Rate, Position, "30", mServo);
            } else {
                _collect->SetTest(Nu, Rate, Position, "30", mServo);
            }
            _collect->TestComeBackToOrigin(mServo);
            _collect->SetVolt(Vn);
            _collect->PowerStart();

            newLifeTesterDialog(fileName, mServo);
        }
    }
}

void Widget::recordValue(int d, QString pf, QString In, QString Nu, QString Ra, QString Po, QString Ti)
{
    UART_t tempUart;
    tempUart.data = devInformation.at(d).data;
    tempUart.com = devInformation.at(d).com;
    tempUart.value.Pf = pf;
    tempUart.value.In = In;
    tempUart.value.Nu = Nu;
    tempUart.value.Ra = Ra;
    tempUart.value.Po = Po;
    tempUart.value.Ti = Ti;
    devInformation.removeAt(d);
    devInformation.insert(d,tempUart);
}
