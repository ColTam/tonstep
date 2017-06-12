﻿#include "formdisplay.h"
#include "ui_formdisplay.h"
#include "qmessagewidget.h"
#include "Collect.h"
#include "pathdialog.h"
#include "nofocusdelegate.h"

#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QTimer>
#include <QStringListIterator>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QSettings>
#include <QMenu>
#include <QTranslator>

#include <helpWidget/helpwidget.h>

QString FormDisplay::dirName = "";

FormDisplay::FormDisplay(QWidget *parent) :
    QWidget(parent)
  , ui(new Ui::FormDisplay)
  , isOld(PROJECT_CONDITION_NEW)
  , isAuto(0), isManual(0), mServo(servoA)
  , mPiecesViewListA(nullptr)
  , mPiecesPowerListA(nullptr)
  , mPiecesViewListB(nullptr)
  , mPiecesPowerListB(nullptr)
  , mPiecesViewListC(nullptr)
  , mPiecesPowerListC(nullptr)
{
    ui->setupUi(this);
    //value
    mFileNameListA.clear();
    mFileNameListB.clear();
    mFileNameListC.clear();
    //widget
    ui->lineEdit_itemId->setContextMenuPolicy(Qt::NoContextMenu);
    ui->lineEdit_itemId->setMaxLength(ITEM_ID_LENGTH);
    ui->tabWidget->setCurrentIndex(TABWIDGET_SERVOA);
    //login
    ui->stackedWidget->setCurrentIndex(STACKWIDGET_LOGIN);
    ui->lineEdit_itemId->setFocus();
    ui->lineEdit_itemId->selectAll();
    {//special mode --unused
        ui->checkBox_breaking->setEnabled(false);
        ui->checkBox_temp->setEnabled(false);
        ui->checkBox_breaking_2->setEnabled(false);
        ui->checkBox_temp_2->setEnabled(false);
        ui->checkBox_normal->setEnabled(false);
        ui->checkBox_normal_2->setEnabled(false);
        ui->checkBox_normalTemp->setEnabled(false);
    }
    //clause 19
    ui->label_tempIm->hide();
    ui->lineEdit_tempIm->hide();
    ui->tableWidget_temp->setColumnWidth(0, 47);
    ui->tableWidget_temp->setColumnWidth(2, 40);
    ui->tableWidget_temp->setColumnWidth(3, 58);
    ui->tableWidget_temp->setItemDelegate(new NoFocusDelegate());
    ui->toolButton_save19->setEnabled(false);
    ui->tableWidget_3->horizontalHeader()->setVisible(true);
    ui->tableWidget_3->setItemDelegate(new NoFocusDelegate());
    ui->lineEdit_tempVn_t->setReadOnly(true);
    ui->lineEdit_tempIn_t->setReadOnly(true);
//    ui->tableWidget_temp->setColumnWidth(2, 40);

    //clause 20
    ui->toolButton_save20->setEnabled(false);

    //clause 21
    ui->toolButton_save21->setEnabled(false);
    ui->tableWidget_4->horizontalHeader()->setVisible(true);
    ui->tableWidget_4->setItemDelegate(new NoFocusDelegate());

    //clause 22
    ui->tableWidget_normalTemp->setColumnWidth(0, 47);
    ui->tableWidget_normalTemp->setColumnWidth(2, 40);
    ui->tableWidget_normalTemp->setColumnWidth(3, 58);
    ui->tableWidget_normalTemp->setItemDelegate(new NoFocusDelegate());
    ui->toolButton_save22->setEnabled(false);
    ui->tableWidget_5->horizontalHeader()->setVisible(true);
    ui->tableWidget_5->setItemDelegate(new NoFocusDelegate());

    //clause 19_2
    ui->tableWidget_temp_2->setColumnWidth(0, 47);
    ui->tableWidget_temp_2->setColumnWidth(2, 40);
    ui->tableWidget_temp_2->setColumnWidth(3, 58);
    ui->tableWidget_temp_2->setItemDelegate(new NoFocusDelegate());
    ui->toolButton_save19_2->setEnabled(false);

    //clause 20_2
    ui->toolButton_save20_2->setEnabled(false);

    //clause 21_2
    ui->toolButton_save21_2->setEnabled(false);

    //auto
    ui->tableWidget->setColumnWidth(0, 47);
    ui->tableWidget->setColumnWidth(2, 40);
    ui->tableWidget->setColumnWidth(3, 58);
    ui->tableWidget->setItemDelegate(new NoFocusDelegate());
    ui->tableWidget_2->setColumnWidth(0, 47);
    ui->tableWidget_2->setColumnWidth(2, 40);
    ui->tableWidget_2->setColumnWidth(3, 58);
    ui->tableWidget_2->setItemDelegate(new NoFocusDelegate());
    ui->tableWidget_6->horizontalHeader()->setVisible(true);
    ui->tableWidget_6->setItemDelegate(new NoFocusDelegate());

    //small message
    mMessage = new QMessageWidget(this);
    mMessage->setMinimumSize(958,26);
    mMessage->setGeometry(1,646,958,26);
    //    mMessage->hide();

    m_helpWidget = new HelpWidget(this);
    m_helpWidget->setGeometry(7, 7, 943, 633);
    m_helpWidget->hide();
    if (connectSTAS) {
        mMessage->show();
        mMessage->setText(tr("Connect STAS Sucess !"));
    } else {
        mMessage->show();
        mMessage->setText(tr("Connect STAS False !"));
    }
    Collect::stateStand();

    //pull-down menu
    mPathDialog = new pathDialog();
    mRegistration = new RegistrationCode();

    popMenu = new QMenu(this);
    popMenu->setStyleSheet("QMenu {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FF383838, stop:1 #FF282828);"
                           "border:1px solid #727272; border-radius: 3px;"
                           "}"
                           "QMenu::item {color: #F0F0F0;height: 21px;padding: 0px 23px 0px 10px;"
                           "}"
                           "QMenu::item:selected {"
                           "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #FF646464, stop:1 #FF545454);"
                           "}");

    englishAction = new QAction(this);
    englishAction->setText(tr("English"));
//    pathAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_E));

    chineseAction = new QAction(this);
    chineseAction->setText(tr("Chinese"));

    pathAction = new QAction(this);
    pathAction->setText(tr("Path"));

    RegistrationAction = new QAction(this);
    RegistrationAction->setText(tr("Registration Code"));

    connect(englishAction, SIGNAL(triggered(bool)), this, SLOT(clickedEnglishAction()));
    connect(chineseAction, SIGNAL(triggered(bool)), this, SLOT(clickedChineseAction()));
    connect(pathAction, &QAction::triggered, [this]() {
        mPathDialog->defaultPathLineEdit->setText(dirName);
        mPathDialog->currentPathLineEdit->setText(dirName);
        mPathDialog->exec();
    });
    connect(RegistrationAction, &QAction::triggered, [this]() {
        mRegistration->m_lineEdit->setText(Collect::registrNumber);
        mRegistration->exec();
    });

    popMenu->addAction(englishAction);
    popMenu->addAction(chineseAction);
    popMenu->addAction(pathAction);
    popMenu->addAction(RegistrationAction);

    ui->pushButton_6->setMenu(popMenu);
}

FormDisplay::~FormDisplay()
{
    if (connectSTAS) {
        Collect::stateAllStop();
        Collect::TestDisplayOpen();
    }

    if (mPiecesPowerListC != NULL)
        delete mPiecesPowerListC;
    if (mPiecesViewListC != NULL)
        delete mPiecesViewListC;
    if (mPiecesPowerListB != NULL)
        delete mPiecesPowerListB;
    if (mPiecesViewListB != NULL)
        delete mPiecesViewListB;
    if (mPiecesPowerListA != NULL)
        delete mPiecesPowerListA;
    if (mPiecesViewListA != NULL)
        delete mPiecesViewListA;

    delete mRegistration;
    delete mPathDialog;
    delete ui;
}

void FormDisplay::addItemIcon(QString Image, int goal)
{
    mImage = QPixmap(Image);
    mItemTextList.clear();

    char suffix = '1';
    switch (goal) {
    case 19:
    {
        while (1)
        {
            if (isOld) {
                QFile itemFile_19(dirName+oldItemName);
                itemFile_19.open(QIODevice::Text | QFile::WriteOnly | QIODevice::Truncate);
                if (itemFile_19.isOpen()) {
                    clause19_out(&itemFile_19);
                    itemFile_19.close();
                }
                isOld = PROJECT_CONDITION_NEW;
                break;
            }

            if (mServo == servoA) {
                QString name;
                if (isAuto) {
                    name = "clause19_A0";
                    int i = mFileNameListA.indexOf(name);
                    if (i >= 0) {
                        mFileNameListA.removeAt(i);
                        delete mPiecesViewListA->item(i);
                    }
                } else {
                    name = QString("clause19_A%1").arg(suffix++);
                }

                if (mFileNameListA.indexOf(name) < 0) {
                    mFileNameListA << name;
                    mPiecesViewListA->addPiece(mImage, name);

                    QFile itemFile_19(dirName+name);
                    itemFile_19.open(QIODevice::Text | QFile::WriteOnly | QIODevice::Truncate);
                    if (itemFile_19.isOpen()) {
                        clause19_out(&itemFile_19);
                        itemFile_19.close();
                    }
                    break;
                }
            } else if (mServo == servoB) {
                QString name;
                if (isAuto) {
                    name = "clause19_B0";
                    int i = mFileNameListB.indexOf(name);
                    if (i >= 0) {
                        mFileNameListB.removeAt(i);
                        delete mPiecesViewListB->item(i);
                    }
                } else {
                    name = QString("clause19_B%1").arg(suffix++);
                }

                if (mFileNameListB.indexOf(name) < 0) {
                    mFileNameListB << name;
                    mPiecesViewListB->addPiece(mImage, name);

                    QFile itemFile_19(dirName+name);
                    itemFile_19.open(QIODevice::Text | QFile::WriteOnly | QIODevice::Truncate);
                    if (itemFile_19.isOpen()) {
                        clause19_out(&itemFile_19);
                        itemFile_19.close();
                    }
                    break;
                }
            } else if (mServo == servoC) {
                QString name;
                if (isAuto) {
                    name = "clause19_C0";
                    int i = mFileNameListC.indexOf(name);
                    if (i >= 0) {
                        mFileNameListC.removeAt(i);
                        delete mPiecesViewListC->item(i);
                    }
                } else {
                    name = QString("clause19_C%1").arg(suffix++);
                }

                if (mFileNameListC.indexOf(name) < 0) {
                    mFileNameListC << name;
                    mPiecesViewListC->addPiece(mImage, name);

                    QFile itemFile_19(dirName+name);
                    itemFile_19.open(QIODevice::Text | QFile::WriteOnly | QIODevice::Truncate);
                    if (itemFile_19.isOpen()) {
                        clause19_out(&itemFile_19);
                        itemFile_19.close();
                    }
                    break;
                }
            }
        }
        break;
    }
    case 20:
    {
        while (1)
        {
            if (isOld) {
                QFile itemFile_20(dirName+oldItemName);
                itemFile_20.open(QIODevice::Text | QFile::WriteOnly | QIODevice::Truncate);
                if (itemFile_20.isOpen()) {
                    clause20_out(&itemFile_20);
                    itemFile_20.close();
                }
                isOld = PROJECT_CONDITION_NEW;
                break;
            }

            if (mServo == servoA) {
                QString name;
                if (isAuto) {
                    name = "clause20_A0";
                    int i = mFileNameListA.indexOf(name);
                    if (i > 0) {
                        mFileNameListA.removeAt(i);
                        delete mPiecesViewListA->item(i);
                    }
                } else {
                    name = QString("clause20_A%1").arg(suffix++);
                }

                if (mFileNameListA.indexOf(name) < 0) {
                    mFileNameListA << name;
                    mPiecesViewListA->addPiece(mImage, name);

                    QFile itemFile_20(dirName+name);
                    itemFile_20.open(QIODevice::Text | QFile::WriteOnly | QIODevice::Truncate);
                    if (itemFile_20.isOpen()) {
                        clause20_out(&itemFile_20);
                        itemFile_20.close();
                    }
                    break;
                }
            } else if (mServo == servoB) {
                QString name;
                if (isAuto) {
                    name = "clause20_B0";
                    int i = mFileNameListB.indexOf(name);
                    if (i > 0) {
                        mFileNameListB.removeAt(i);
                        delete mPiecesViewListB->item(i);
                    }
                } else {
                    name = QString("clause20_B%1").arg(suffix++);
                }

                if (mFileNameListB.indexOf(name) < 0) {
                    mFileNameListB << name;
                    mPiecesViewListB->addPiece(mImage, name);

                    QFile itemFile_20(dirName+name);
                    itemFile_20.open(QIODevice::Text | QFile::WriteOnly | QIODevice::Truncate);
                    if (itemFile_20.isOpen()) {
                        clause20_out(&itemFile_20);
                        itemFile_20.close();
                    }
                    break;
                }
            } else if (mServo == servoC) {
                QString name;
                if (isAuto) {
                    name = "clause20_C0";
                    int i = mFileNameListC.indexOf(name);
                    if (i > 0) {
                        mFileNameListC.removeAt(i);
                        delete mPiecesViewListC->item(i);
                    }
                } else {
                    name = QString("clause20_C%1").arg(suffix++);
                }

                if (mFileNameListC.indexOf(name) < 0) {
                    mFileNameListC << name;
                    mPiecesViewListC->addPiece(mImage, name);

                    QFile itemFile_20(dirName+name);
                    itemFile_20.open(QIODevice::Text | QFile::WriteOnly | QIODevice::Truncate);
                    if (itemFile_20.isOpen()) {
                        clause20_out(&itemFile_20);
                        itemFile_20.close();
                    }
                    break;
                }
            }
        }
        break;
    }
    case 21:
    {
        while (1)
        {
            if (isOld) {
                QFile itemFile_21(dirName+oldItemName);
                itemFile_21.open(QIODevice::Text | QFile::WriteOnly | QIODevice::Truncate);
                if (itemFile_21.isOpen()) {
                    clause21_out(&itemFile_21);
                    itemFile_21.close();
                }
                isOld = PROJECT_CONDITION_NEW;
                break;
            }

            if (mServo == servoA) {
                QString name;
                if (isAuto) {
                    name = "clause21_A0";
                    int i = mFileNameListA.indexOf(name);
                    if (i > 0) {
                        mFileNameListA.removeAt(i);
                        delete mPiecesViewListA->item(i);
                    }
                } else {
                    name = QString("clause21_A%1").arg(suffix++);
                }

                if (mFileNameListA.indexOf(name) < 0) {
                    mFileNameListA << name;
                    mPiecesViewListA->addPiece(mImage, name);

                    QFile itemFile_21(dirName+name);
                    itemFile_21.open(QIODevice::Text | QFile::WriteOnly | QIODevice::Truncate);
                    if (itemFile_21.isOpen()) {
                        clause21_out(&itemFile_21);
                        itemFile_21.close();
                    }
                    break;
                }
            } else if (mServo == servoB) {
                QString name;
                if (isAuto) {
                    name = "clause21_B0";
                    int i = mFileNameListB.indexOf(name);
                    if (i > 0) {
                        mFileNameListB.removeAt(i);
                        delete mPiecesViewListB->item(i);
                    }
                } else {
                    name = QString("clause21_B%1").arg(suffix++);
                }

                if (mFileNameListB.indexOf(name) < 0) {
                    mFileNameListB << name;
                    mPiecesViewListB->addPiece(mImage, name);

                    QFile itemFile_21(dirName+name);
                    itemFile_21.open(QIODevice::Text | QFile::WriteOnly | QIODevice::Truncate);
                    if (itemFile_21.isOpen()) {
                        clause21_out(&itemFile_21);
                        itemFile_21.close();
                    }
                    break;
                }
            } else if (mServo == servoC) {
                QString name;
                if (isAuto) {
                    name = "clause21_C0";
                    int i = mFileNameListC.indexOf(name);
                    if (i > 0) {
                        mFileNameListC.removeAt(i);
                        delete mPiecesViewListC->item(i);
                    }
                } else {
                    name = QString("clause21_C%1").arg(suffix++);
                }

                if (mFileNameListC.indexOf(name) < 0) {
                    mFileNameListC << name;
                    mPiecesViewListC->addPiece(mImage, name);

                    QFile itemFile_21(dirName+name);
                    itemFile_21.open(QIODevice::Text | QFile::WriteOnly | QIODevice::Truncate);
                    if (itemFile_21.isOpen()) {
                        clause21_out(&itemFile_21);
                        itemFile_21.close();
                    }
                    break;
                }
            }
        }
        break;
    }
    case 22:
    {
        while (1)
        {
            if (isOld) {
                QFile itemFile_22(dirName+oldItemName);
                itemFile_22.open(QIODevice::Text | QFile::WriteOnly | QIODevice::Truncate);
                if (itemFile_22.isOpen()) {
                    clause22_out(&itemFile_22);
                    itemFile_22.close();
                }
                isOld = PROJECT_CONDITION_NEW;
                break;
            }

            if (mServo == servoA) {
                QString name;
                if (isAuto) {
                    name = "clause22_A0";
                    int i = mFileNameListA.indexOf(name);
                    if (i > 0) {
                        mFileNameListA.removeAt(i);
                        delete mPiecesViewListA->item(i);
                    }
                } else {
                    name = QString("clause22_A%1").arg(suffix++);
                }

                if (mFileNameListA.indexOf(name) < 0) {
                    mFileNameListA << name;
                    mPiecesViewListA->addPiece(mImage, name);

                    QFile itemFile_22(dirName+name);
                    itemFile_22.open(QIODevice::Text | QFile::WriteOnly | QIODevice::Truncate);
                    if (itemFile_22.isOpen()) {
                        clause22_out(&itemFile_22);
                        itemFile_22.close();
                    }
                    break;
                }
            } else if (mServo == servoB) {
                QString name;
                if (isAuto) {
                    name = "clause22_B0";
                    int i = mFileNameListB.indexOf(name);
                    if (i > 0) {
                        mFileNameListB.removeAt(i);
                        delete mPiecesViewListB->item(i);
                    }
                } else {
                    name = QString("clause22_B%1").arg(suffix++);
                }

                if (mFileNameListB.indexOf(name) < 0) {
                    mFileNameListB << name;
                    mPiecesViewListB->addPiece(mImage, name);

                    QFile itemFile_22(dirName+name);
                    itemFile_22.open(QIODevice::Text | QFile::WriteOnly | QIODevice::Truncate);
                    if (itemFile_22.isOpen()) {
                        clause22_out(&itemFile_22);
                        itemFile_22.close();
                    }
                    break;
                }
            } else if (mServo == servoC) {
                QString name;
                if (isAuto) {
                    name = "clause22_C0";
                    int i = mFileNameListC.indexOf(name);
                    if (i > 0) {
                        mFileNameListC.removeAt(i);
                        delete mPiecesViewListC->item(i);
                    }
                } else {
                    name = QString("clause22_C%1").arg(suffix++);
                }

                if (mFileNameListC.indexOf(name) < 0) {
                    mFileNameListC << name;
                    mPiecesViewListC->addPiece(mImage, name);

                    QFile itemFile_22(dirName+name);
                    itemFile_22.open(QIODevice::Text | QFile::WriteOnly | QIODevice::Truncate);
                    if (itemFile_22.isOpen()) {
                        clause22_out(&itemFile_22);
                        itemFile_22.close();
                    }
                    break;
                }
            }
        }
        break;
    }
    }

    if (isAuto) isAuto = false;

    qSort(mFileNameListA.begin(), mFileNameListA.end());
    qSort(mFileNameListB.begin(), mFileNameListB.end());
    qSort(mFileNameListC.begin(), mFileNameListC.end());
    ui->stackedWidget->setCurrentIndex(STACKWIDGET_MAIN);
}

bool FormDisplay::isChecked(QTableWidgetItem *item)
{
    if (item->checkState() == Qt::Checked)
        return true;

    return false;
}

void FormDisplay::initMPiece()
{
    mPiecesViewListA = new piecesList(ITEM_ICON_SIZE, ITEM_AUTOSORT, ui->stackedWidget);
    ui->verticalLayout_list->addWidget(mPiecesViewListA);

    mPiecesPowerListA = new piecesList(ITEM_ICON_SIZE, ITEM_NAUTOSORT, ui->stackedWidget);
    ui->verticalLayout_power->addWidget(mPiecesPowerListA);

    mPiecesViewListB = new piecesList(ITEM_ICON_SIZE, ITEM_AUTOSORT, ui->stackedWidget);
    ui->verticalLayout_list_2->addWidget(mPiecesViewListB);

    mPiecesPowerListB = new piecesList(ITEM_ICON_SIZE, ITEM_NAUTOSORT, ui->stackedWidget);
    ui->verticalLayout_power_2->addWidget(mPiecesPowerListB);

    mPiecesViewListC = new piecesList(ITEM_ICON_SIZE, ITEM_AUTOSORT, ui->stackedWidget);
    ui->verticalLayout_list_3->addWidget(mPiecesViewListC);

    mPiecesPowerListC = new piecesList(ITEM_ICON_SIZE, ITEM_NAUTOSORT, ui->stackedWidget);
    ui->verticalLayout_power_3->addWidget(mPiecesPowerListC);
}

void FormDisplay::uartTips()
{
    QString tips;
    if (devInformation.at(0).data.isEmpty()){
        ui->checkBox_VOLT->setChecked(false);
        tips += tr("Power Supply,");
    } else {
        ui->checkBox_VOLT->setChecked(true);
    }

    if (devInformation.at(1).data.isEmpty()){
        ui->checkBox_TRC->setChecked(false);
        tips += tr("Paperless Recorder,");
    } else {
        ui->checkBox_TRC->setChecked(true);
        QStringList readList;
        readList << devInformation.at(1).data.split("TT");

        ui->textEdit->clear();
        ui->textEdit_2->clear();
        ui->textEdit_3->clear();
        ui->textEdit_4->clear();
        ui->textEdit_5->clear();
        Collect::getTemperatureRise(&readList);
        for (int i = 0; i < readList.count(); i++)
        {
            QString tc = readList.at(i);
            double value = tc.mid(19).toDouble();
            if (value < 0 || value >= 80) {
                ui->textEdit->append("CH"+tc.mid(2,4)+tr("disabled"));
                ui->textEdit_2->append("CH"+tc.mid(2,4)+tr("disabled"));
                ui->textEdit_3->append("CH"+tc.mid(2,4)+tr("disabled"));
                ui->textEdit_4->append("CH"+tc.mid(2,4)+tr("disabled"));
                ui->textEdit_5->append("CH"+tc.mid(2,4)+tr("disabled"));
            }
        }
    }

    if (devInformation.at(2).data.isEmpty()){
        ui->checkBox_A->setChecked(false);
        tips += tr("Load (Servo-A),");
    } else {
        ui->checkBox_A->setChecked(true);
    }

    if (devInformation.at(3).data.isEmpty()){
        ui->checkBox_B->setChecked(false);
        tips += tr("Load (Servo-B),");
    } else {
        ui->checkBox_B->setChecked(true);
    }

    if (devInformation.at(4).data.isEmpty()){
        ui->checkBox_C->setChecked(false);
        tips += tr("Load (Servo-C),");
    } else {
        ui->checkBox_C->setChecked(true);
    }

    if (devInformation.at(5).data.isEmpty()){
        ui->checkBox_TEST->setChecked(false);
        ui->checkBox_TEST_A->setChecked(false);
        tips += tr("Life Tester (servoA),");
    } else {
        ui->checkBox_TEST->setChecked(true);
        ui->checkBox_TEST_A->setChecked(true);
    }

    if (devInformation.at(6).data.isEmpty()){
        ui->checkBox_TEST->setChecked(false);
        ui->checkBox_TEST_B->setChecked(false);
        tips += tr("Life Tester (servoB),");
    } else {
        ui->checkBox_TEST->setChecked(true);
        ui->checkBox_TEST_B->setChecked(true);
    }

    if (devInformation.at(7).data.isEmpty()){
        ui->checkBox_TEST->setChecked(false);
        ui->checkBox_TEST_C->setChecked(false);
        tips += tr("Life Tester (servoC),");
    } else {
        ui->checkBox_TEST->setChecked(true);
        ui->checkBox_TEST_C->setChecked(true);
    }
    ui->pushButton_4->setEnabled(false);

    if (!tips.isEmpty()) {
        QMessageBox msgBox(QMessageBox::Warning,tr(" Electrical Accessories Test Automation Program"),
                           tips + tr("Connection fail! Check the serial port, please."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
        msgBox.setWindowIcon(QIcon(WINDOW_ICON));

        msgBox.exec();
    }
}

void FormDisplay::enterMain()
{
    if (ui->lineEdit_itemId->text().isEmpty()){//判断项目ID是否填写
        mMessage->show();
        mMessage->setText(tr("Please Enter The Project ID."));
        ui->lineEdit_itemId->setFocus();
        ui->lineEdit_itemId->selectAll();
        return;
    }

    fileSize = dirName.size();//判断目录情况，是否存在，存在则提示进入，否则直接创建
    QDir workDir(dirName);
    if (workDir.exists(ui->lineEdit_itemId->text())){
        QMessageBox msgBox(QMessageBox::Question,tr("Question"),tr("Project has been in existence, whether to continue?"));
        msgBox.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
        msgBox.setButtonText(QMessageBox::Cancel,tr("Cancel"));
        msgBox.setWindowIcon(QIcon(WINDOW_ICON));

        int ret = msgBox.exec();

        if (ret == QMessageBox::Cancel) {
            return;
        }

        workDir.cd(ui->lineEdit_itemId->text());

        if (mPlug == 0) {
            if (workDir.exists("IEC60320/cfg")){
                workDir.cd("IEC60320/cfg");
            }else {
                workDir.mkdir("IEC60320/cfg");
                workDir.cd("IEC60320/cfg");
            }
        } else if (mPlug == 1) {
            if (workDir.exists("IEC60884/cfg")){
                workDir.cd("IEC60884/cfg");
            }else {
                workDir.mkdir("IEC60884/cfg");
                workDir.cd("IEC60884/cfg");
            }
        }

        initMPiece();

        QStringList oldFile = workDir.entryList(workDir.filter());
        QListIterator<QString> i(oldFile);

        while(i.hasNext())
        {
            QString test = i.next();
            if (test.startsWith("clause") && test.right(3).mid(0,1) == "_")
            {
                switch (test.mid(6, 2).toInt()) {
                case 19:
                {
                    if (test.mid(9, 1) == "A") {
                        mFileNameListA << test.left(11);
                        mPiecesViewListA->addPiece(QPixmap(IMAGE_CLAUSE19), test.left(11));
                    } else if (test.mid(9, 1) == "B") {
                        mFileNameListB << test.left(11);
                        mPiecesViewListB->addPiece(QPixmap(IMAGE_CLAUSE19), test.left(11));
                    } else if (test.mid(9, 1) == "C") {
                        mFileNameListC << test.left(11);
                        mPiecesViewListC->addPiece(QPixmap(IMAGE_CLAUSE19), test.left(11));
                    }

                    break;
                }
                case 20:
                {
                    if (test.mid(9, 1) == "A") {
                        mFileNameListA << test.left(11);
                        mPiecesViewListA->addPiece(QPixmap(IMAGE_CLAUSE20), test.left(11));
                    } else if (test.mid(9, 1) == "B") {
                        mFileNameListB << test.left(11);
                        mPiecesViewListB->addPiece(QPixmap(IMAGE_CLAUSE20), test.left(11));
                    } else if (test.mid(9, 1) == "C") {
                        mFileNameListC << test.left(11);
                        mPiecesViewListC->addPiece(QPixmap(IMAGE_CLAUSE20), test.left(11));
                    }
                    break;
                }
                case 21:
                {
                    if (test.mid(9, 1) == "A"){
                        mFileNameListA << test.left(11);
                        mPiecesViewListA->addPiece(QPixmap(IMAGE_CLAUSE21), test.left(11));
                    } else if (test.mid(9, 1) == "B") {
                        mFileNameListB << test.left(11);
                        mPiecesViewListB->addPiece(QPixmap(IMAGE_CLAUSE21), test.left(11));
                    } else if (test.mid(9, 1) == "C") {
                        mFileNameListC << test.left(11);
                        mPiecesViewListC->addPiece(QPixmap(IMAGE_CLAUSE21), test.left(11));
                    }
                    break;
                }
                case 22:
                {
                    if (test.mid(9, 1) == "A") {
                        mFileNameListA << test.left(11);
                        mPiecesViewListA->addPiece(QPixmap(IMAGE_CLAUSE22), test.left(11));
                    } else if (test.mid(9, 1) == "B") {
                        mFileNameListB << test.left(11);
                        mPiecesViewListB->addPiece(QPixmap(IMAGE_CLAUSE22), test.left(11));
                    } else if (test.mid(9, 1) == "C") {
                        mFileNameListC << test.left(11);
                        mPiecesViewListC->addPiece(QPixmap(IMAGE_CLAUSE22), test.left(11));
                    }
                    break;
                }
                }
            }
        }
    } else {
        workDir.mkpath("./"+ui->lineEdit_itemId->text()+"/IEC60320/cfg");
        workDir.mkpath("./"+ui->lineEdit_itemId->text()+"/IEC60320/data/beforeTest/servoA");
        workDir.mkpath("./"+ui->lineEdit_itemId->text()+"/IEC60320/data/beforeTest/servoB");
        workDir.mkpath("./"+ui->lineEdit_itemId->text()+"/IEC60320/data/beforeTest/servoC");
        workDir.mkpath("./"+ui->lineEdit_itemId->text()+"/IEC60320/data/afterTest/servoA");
        workDir.mkpath("./"+ui->lineEdit_itemId->text()+"/IEC60320/data/afterTest/servoB");
        workDir.mkpath("./"+ui->lineEdit_itemId->text()+"/IEC60320/data/afterTest/servoC");
        workDir.mkpath("./"+ui->lineEdit_itemId->text()+"/IEC60884/cfg");
        workDir.mkpath("./"+ui->lineEdit_itemId->text()+"/IEC60884/data/beforeTest/servoA");
        workDir.mkpath("./"+ui->lineEdit_itemId->text()+"/IEC60884/data/beforeTest/servoB");
        workDir.mkpath("./"+ui->lineEdit_itemId->text()+"/IEC60884/data/beforeTest/servoC");
        workDir.mkpath("./"+ui->lineEdit_itemId->text()+"/IEC60884/data/afterTest/servoA");
        workDir.mkpath("./"+ui->lineEdit_itemId->text()+"/IEC60884/data/afterTest/servoB");
        workDir.mkpath("./"+ui->lineEdit_itemId->text()+"/IEC60884/data/afterTest/servoC");

        if (mPlug == 0) {
            workDir.cd("./"+ui->lineEdit_itemId->text()+"/IEC60320/cfg");
        } else if (mPlug == 1) {
            workDir.cd("./"+ui->lineEdit_itemId->text()+"/IEC60884/cfg");
        }

        initMPiece();
    }

    dirName = workDir.path() + "/";

    switch (mPlug) {
    case IEC60320:
    {
        ui->toolButton_22->hide();
        ui->toolButton_26->hide();
        ui->toolButton_30->hide();
        mMessage->show();
        mMessage->setText("IEC60320: "+ui->lineEdit_itemId->text());
        ui->groupBox_4->setTitle("IEC60320");
        ui->groupBox_5->setTitle("IEC60320");
        ui->groupBox_6->setTitle("IEC60320");
        break;
    }
    case IEC60884:
    {
        ui->toolButton_22->show();
        ui->toolButton_26->show();
        ui->toolButton_30->show();
        mMessage->show();
        mMessage->setText("IEC60884: "+ui->lineEdit_itemId->text());
        ui->groupBox_4->setTitle("IEC60884");
        ui->groupBox_5->setTitle("IEC60884");
        ui->groupBox_6->setTitle("IEC60884");
        break;
    }
    }

    connect(mPiecesViewListA, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(setTheItem(QListWidgetItem*)));
    connect(mPiecesViewListA, SIGNAL(moveItem(QListWidgetItem*)), this, SLOT(moveToZone(QListWidgetItem*)));
    connect(mPiecesViewListA, SIGNAL(changeItem(QListWidgetItem*)), this, SLOT(setTheItem(QListWidgetItem*)));
    connect(mPiecesViewListA, SIGNAL(deleteItem(QListWidgetItem*)), this, SLOT(delItem(QListWidgetItem*)));
    connect(mPiecesPowerListA, SIGNAL(itemDoubleClicked(QListWidgetItem*)),this, SLOT(itemRestore(QListWidgetItem*)));

    connect(mPiecesViewListB, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(setTheItem(QListWidgetItem*)));
    connect(mPiecesViewListB, SIGNAL(moveItem(QListWidgetItem*)), this, SLOT(moveToZone(QListWidgetItem*)));
    connect(mPiecesViewListB, SIGNAL(changeItem(QListWidgetItem*)), this, SLOT(setTheItem(QListWidgetItem*)));
    connect(mPiecesViewListB, SIGNAL(deleteItem(QListWidgetItem*)), this, SLOT(delItem(QListWidgetItem*)));
    connect(mPiecesPowerListB, SIGNAL(itemDoubleClicked(QListWidgetItem*)),this, SLOT(itemRestore(QListWidgetItem*)));

    connect(mPiecesViewListC, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(setTheItem(QListWidgetItem*)));
    connect(mPiecesViewListC, SIGNAL(moveItem(QListWidgetItem*)), this, SLOT(moveToZone(QListWidgetItem*)));
    connect(mPiecesViewListC, SIGNAL(changeItem(QListWidgetItem*)), this, SLOT(setTheItem(QListWidgetItem*)));
    connect(mPiecesViewListC, SIGNAL(deleteItem(QListWidgetItem*)), this, SLOT(delItem(QListWidgetItem*)));
    connect(mPiecesPowerListC, SIGNAL(itemDoubleClicked(QListWidgetItem*)),this, SLOT(itemRestore(QListWidgetItem*)));

    //main
    ui->stackedWidget->setCurrentIndex(STACKWIDGET_MAIN);
    ui->tabWidget->setCurrentIndex(0);

    ui->lineEdit_LOADUART_A->setText("COM"+QString::number(comLoadA));
    ui->lineEdit_LOADUART_B->setText("COM"+QString::number(comLoadB));
    ui->lineEdit_LOADUART_C->setText("COM"+QString::number(comLoadC));
    ui->lineEdit_VOLTUART->setText("COM"+QString::number(comVolt));
    ui->lineEdit_TESTUART->setText("COM"+QString::number(comTest));
    ui->lineEdit_TRCUART->setText("COM"+QString::number(comTRC));

    m_helpWidget->show();
    uartTips();
}

void FormDisplay::delItem(QListWidgetItem *item)
{
    QMessageBox msgBox(QMessageBox::Information,tr("Delete"),tr("Please make sure to delete the item!"));
    msgBox.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
    msgBox.setButtonText(QMessageBox::Cancel,tr("Cancel"));
    msgBox.setWindowIcon(QIcon(WINDOW_ICON));

    int ret = msgBox.exec();

    if (ret == QMessageBox::Cancel) {
        return;
    }

    QFile delFile(dirName+item->text());
    delFile.remove();

    if (mServo == servoA) {
        mFileNameListA.removeOne(item->text());
    } else if (mServo == servoB) {
        mFileNameListB.removeOne(item->text());
    } else if (mServo == servoC) {
        mFileNameListC.removeOne(item->text());
    }

    delete item;
}

void FormDisplay::itemRestore(QListWidgetItem *item)
{
    switch (item->text().mid(6, 2).toInt()) {
    case 19:
    {
        if (item->text().right(2).mid(0,1) == "A") {
            mPiecesViewListA->addPiece(QPixmap(IMAGE_CLAUSE19), item->text());
        } else if (item->text().right(2).mid(0,1) == "B") {
            mPiecesViewListB->addPiece(QPixmap(IMAGE_CLAUSE19), item->text());
        } else if (item->text().right(2).mid(0,1) == "C") {
            mPiecesViewListC->addPiece(QPixmap(IMAGE_CLAUSE19), item->text());
        }
        break;
    }
    case 20:
    {
        if (item->text().right(2).mid(0,1) == "A") {
            mPiecesViewListA->addPiece(QPixmap(IMAGE_CLAUSE20), item->text());
        } else if (item->text().right(2).mid(0,1) == "B") {
            mPiecesViewListB->addPiece(QPixmap(IMAGE_CLAUSE20), item->text());
        } else if (item->text().right(2).mid(0,1) == "C") {
            mPiecesViewListC->addPiece(QPixmap(IMAGE_CLAUSE20), item->text());
        }
        break;
    }
    case 21:
    {
        if (item->text().right(2).mid(0,1) == "A") {
            mPiecesViewListA->addPiece(QPixmap(IMAGE_CLAUSE21), item->text());
        } else if (item->text().right(2).mid(0,1) == "B") {
            mPiecesViewListB->addPiece(QPixmap(IMAGE_CLAUSE21), item->text());
        } else if (item->text().right(2).mid(0,1) == "C") {
            mPiecesViewListC->addPiece(QPixmap(IMAGE_CLAUSE21), item->text());
        }
        break;
    }
    case 22:
    {
        if (item->text().right(2).mid(0,1) == "A") {
            mPiecesViewListA->addPiece(QPixmap(IMAGE_CLAUSE22), item->text());
        } else if (item->text().right(2).mid(0,1) == "B") {
            mPiecesViewListB->addPiece(QPixmap(IMAGE_CLAUSE22), item->text());
        } else if (item->text().right(2).mid(0,1) == "C") {
            mPiecesViewListC->addPiece(QPixmap(IMAGE_CLAUSE22), item->text());
        }
        break;
    }
    }

    delete item;
}

void FormDisplay::setTheItem(QListWidgetItem *item)
{
    isOld = PROJECT_CONDITION_OLD;
    oldItemName = item->text();
    QFile itemFile(dirName+item->text());
    switch (item->text().mid(6,2).toInt()) {
    case 19:
    {
        clause19_clear();
        clause19_2_clear();
        itemFile.open(QFile::ReadOnly | QIODevice::Text);
        if (itemFile.isOpen()) {
            clause19_in(&itemFile);
            itemFile.close();
        }
        switch (mPlug) {
        case IEC60320:
        {
            ui->stackedWidget->setCurrentIndex(STACKWIDGET_CLAUSE19_320);
            break;
        }
        case IEC60884:
        {
            ui->stackedWidget->setCurrentIndex(STACKWIDGET_CLAUSE19_884);
            break;
        }
        }
        break;
    }
    case 20:
    {
        clause20_clear();
        clause20_2_clear();
        itemFile.open(QFile::ReadOnly | QIODevice::Text);
        if (itemFile.isOpen()) {
            clause20_in(&itemFile);
            itemFile.close();
        }
        switch (mPlug) {
        case IEC60320:
        {
            ui->stackedWidget->setCurrentIndex(STACKWIDGET_CLAUSE20_320);
            break;
        }
        case IEC60884:
        {
            ui->stackedWidget->setCurrentIndex(STACKWIDGET_CLAUSE20_884);
            break;
        }
        }
        break;
    }
    case 21:
    {
        clause21_clear();
        clause21_2_clear();
        itemFile.open(QFile::ReadOnly | QIODevice::Text);
        if (itemFile.isOpen()) {
            clause21_in(&itemFile);
            itemFile.close();
        }
        switch (mPlug) {
        case IEC60320:
        {
            ui->stackedWidget->setCurrentIndex(STACKWIDGET_CLAUSE21_320);
            break;
        }
        case IEC60884:
        {
            ui->stackedWidget->setCurrentIndex(STACKWIDGET_CLAUSE21_884);
            break;
        }
        }
        break;
    }
    case 22:
    {
        clause22_clear();
        itemFile.open(QFile::ReadOnly | QIODevice::Text);
        if (itemFile.isOpen()) {
            clause22_in(&itemFile);
            itemFile.close();
        }
        ui->stackedWidget->setCurrentIndex(STACKWIDGET_CLAUSE22_884);
        break;
    }
    }
}

void FormDisplay::moveToZone(QListWidgetItem *item)
{
    switch (item->text().mid(6, 2).toInt()) {
    case 19:
    {
        if (mServo == servoA) {
            mPiecesPowerListA->addPiece(QPixmap(IMAGE_CLAUSE19), item->text());
        } else if (mServo == servoB) {
            mPiecesPowerListB->addPiece(QPixmap(IMAGE_CLAUSE19), item->text());
        } else if (mServo == servoC) {
            mPiecesPowerListC->addPiece(QPixmap(IMAGE_CLAUSE19), item->text());
        }
        break;
    }
    case 20:
    {
        if (mServo == servoA) {
            mPiecesPowerListA->addPiece(QPixmap(IMAGE_CLAUSE20), item->text());
        } else if (mServo == servoB) {
            mPiecesPowerListB->addPiece(QPixmap(IMAGE_CLAUSE20), item->text());
        } else if (mServo == servoC) {
            mPiecesPowerListC->addPiece(QPixmap(IMAGE_CLAUSE20), item->text());
        }
        break;
    }
    case 21:
    {
        if (mServo == servoA) {
            mPiecesPowerListA->addPiece(QPixmap(IMAGE_CLAUSE21), item->text());
        } else if (mServo == servoB) {
            mPiecesPowerListB->addPiece(QPixmap(IMAGE_CLAUSE21), item->text());
        } else if (mServo == servoC) {
            mPiecesPowerListC->addPiece(QPixmap(IMAGE_CLAUSE21), item->text());
        }
        break;
    }
    case 22:
    {
        if (mServo == servoA) {
            mPiecesPowerListA->addPiece(QPixmap(IMAGE_CLAUSE22), item->text());
        } else if (mServo == servoB) {
            mPiecesPowerListB->addPiece(QPixmap(IMAGE_CLAUSE22), item->text());
        } else if (mServo == servoC) {
            mPiecesPowerListC->addPiece(QPixmap(IMAGE_CLAUSE22), item->text());
        }
        break;
    }
    }

    delete item;
}

void FormDisplay::finishedItem(QString fileName, int number)//fail number
{
    QString _servo = fileName.right(2).mid(0,1);

    if (number == -1) {//用户中断测试
        if (_servo == "A") {
            for (int i = dirNameListA.count(); i > 0; i--) {
                dirNameListA.removeFirst();
                itemRestore(mPiecesPowerListA->item(0));
            }
            ui->toolButton_start->setEnabled(true);
            mPiecesPowerListA->setAcceptDrops(true);
            mPiecesPowerListA->setDragEnabled(true);
        } else if (_servo == "B") {
            for (int i = dirNameListB.count(); i > 0; i--) {
                dirNameListB.removeFirst();
                itemRestore(mPiecesPowerListB->item(0));
            }
            ui->toolButton_start_2->setEnabled(true);
            mPiecesPowerListB->setAcceptDrops(true);
            mPiecesPowerListB->setDragEnabled(true);
        } else if (_servo == "C") {
            for (int i = dirNameListC.count(); i > 0; i--) {
                dirNameListC.removeFirst();
                itemRestore(mPiecesPowerListC->item(0));
            }
            ui->toolButton_start_3->setEnabled(true);
            mPiecesPowerListC->setAcceptDrops(true);
            mPiecesPowerListC->setDragEnabled(true);
        }
    } else {//继续项目测试
        if (fileName.right(5).mid(0,2) == "19" && mPlug == 1) {
            Collect::AlarmOpen();
            QMessageBox msgBox(QMessageBox::Warning,tr(" Electrical Accessories Test Automation Program"),
                               tr("End of the test, please replace the test gauge or plug socket!"));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
            msgBox.setWindowIcon(QIcon(WINDOW_ICON));

            msgBox.exec();
            Collect::AlarmClose();
        }

        if (_servo == "A") {
            dirNameListA.removeFirst();
            itemRestore(mPiecesPowerListA->item(0));
            if (dirNameListA.count() <= 0){
                ui->toolButton_start->setEnabled(true);
                mPiecesPowerListA->setAcceptDrops(true);
                mPiecesPowerListA->setDragEnabled(true);
            }
        } else if (_servo == "B") {
            dirNameListB.removeFirst();
            itemRestore(mPiecesPowerListB->item(0));
            if (dirNameListB.count() <= 0){
                ui->toolButton_start_2->setEnabled(true);
                mPiecesPowerListB->setAcceptDrops(true);
                mPiecesPowerListB->setDragEnabled(true);
            }
        } else if (_servo == "C") {
            dirNameListC.removeFirst();
            itemRestore(mPiecesPowerListC->item(0));
            if (dirNameListC.count() <= 0){
                ui->toolButton_start_3->setEnabled(true);
                mPiecesPowerListC->setAcceptDrops(true);
                mPiecesPowerListC->setDragEnabled(true);
            }
        }
    }

    if (ui->toolButton_start->isEnabled() &&
            ui->toolButton_start_2->isEnabled() &&
            ui->toolButton_start_3->isEnabled()) {//所有测试完成 停止线程读取
        Collect::PowerStop();
        Collect::stateStand();
        emit tTimerStop(0,0);
    } else {//判断未测试项目，只保留未测试数据读取
        int r = 0, e = 0;
        if (dirNameListA.count() > 0 && _servo != "A") {
            if (dirNameListA.count()>0)
            {
                QString s = dirNameListA.at(0);
                if (s.right(5).mid(0,2) == "19" || s.right(5).mid(0,2) == "22") {
                    r++;
                }
            }
            if (dirNameListA.count()>0)
            {
                QString s = dirNameListA.at(0);
                if (s.right(5).mid(0,2) == "20" || s.right(5).mid(0,2) == "21") {
                    e++;
                }
            }
        }
        if (dirNameListB.count() && _servo != "B") {
            if (dirNameListB.count()>0) {
                QString s = dirNameListB.at(0);
                if (s.right(5).mid(0,2) == "19" || s.right(5).mid(0,2) == "22") {
                    r++;
                }
            }
            if (dirNameListB.count()>0) {
                QString s = dirNameListB.at(0);
                if (s.right(5).mid(0,2) == "20" || s.right(5).mid(0,2) == "21") {
                    e++;
                }
            }
        }
        if (dirNameListC.count() && _servo != "C") {
            if (dirNameListC.count()>0) {
                QString s = dirNameListC.at(0);
                if (s.right(5).mid(0,2) == "19" || s.right(5).mid(0,2) == "22") {
                    r++;
                }
            }
            if (dirNameListC.count()>0) {
                QString s = dirNameListC.at(0);
                if (s.right(5).mid(0,2) == "20" || s.right(5).mid(0,2) == "21") {
                    e++;
                }
            }
        }

//        qDebug() << "finished r:" << r
//                 << "finished e:" << e
//                 << "finished ...:" << dirNameListA.count() << dirNameListB.count() << dirNameListC.count();
        emit tTimerStop(r, e);

        if ((_servo == "A" && dirNameListA.count() > 0) ||
                (_servo == "B" && dirNameListB.count() > 0) ||
                (_servo == "C" && dirNameListC.count() > 0)) {
            if (number > 0) {//温升测试number端口Fail， 用户选择是否继续测试
                QMessageBox msgBox(QMessageBox::Warning,tr("Fail"),tr("Temperature rise test result is: Fail!\nWhether to continue to operate?"));
                msgBox.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
                msgBox.setButtonText(QMessageBox::Ok,tr("Yes"));
                msgBox.setButtonText(QMessageBox::Cancel,tr("No"));
                msgBox.setWindowIcon(QIcon(WINDOW_ICON));

                int ret = msgBox.exec();

                if (ret == QMessageBox::Cancel) {
                    if (_servo == "A") {
                        for (int i = dirNameListA.count(); i > 0; i--) {
                            dirNameListA.removeFirst();
                            itemRestore(mPiecesPowerListA->item(0));
                        }
                    } else if (_servo == "B") {
                        for (int i = dirNameListB.count(); i > 0; i--) {
                            dirNameListB.removeFirst();
                            itemRestore(mPiecesPowerListB->item(0));
                        }
                    } else if (_servo == "C") {
                        for (int i = dirNameListC.count(); i > 0; i--) {
                            dirNameListC.removeFirst();
                            itemRestore(mPiecesPowerListC->item(0));
                        }
                    }
                } else {
                    if (_servo == "A") {
                        emit isSectionFileName(dirNameListA.at(0), mPlug);
                    } else if (_servo == "B") {
                        emit isSectionFileName(dirNameListB.at(0), mPlug);
                    } else if (_servo == "C") {
                        emit isSectionFileName(dirNameListC.at(0), mPlug);
                    }
                }
            } else {
                if (_servo == "A") {
                    emit isSectionFileName(dirNameListA.at(0), mPlug);
                } else if (_servo == "B") {
                    emit isSectionFileName(dirNameListB.at(0), mPlug);
                } else if (_servo == "C") {
                    emit isSectionFileName(dirNameListC.at(0), mPlug);
                }
            }
        }
    }
}

void FormDisplay::clause19_out(QIODevice *device)
{
    QTextStream out(device);
    switch (mPlug) {
    case IEC60320:
    {
        QString t2 = ui->lineEdit_tempMaxValue2_2->text();
        if (t2.isEmpty()) {
            t2 = "1";
        }
        out << ui->checkBox_temp_2->isChecked()
            << '\n' << ui->lineEdit_tempVn_2->text()
            << '\n' << ui->lineEdit_tempIn_2->text()
            << '\n' << ui->lineEdit_tempH_2->text()
            << '\n' << ui->lineEdit_tempVn_t_2->text()
            << '\n' << ui->lineEdit_tempIn_t_2->text()
            << '\n' << t2;
        for (int i = 0; i<20; i++)
        {
            mItemTextList << ui->tableWidget_temp_2->item(i, 1)->text();
            out << '\n' << this->isChecked(ui->tableWidget_temp_2->item(i, 1))
                << mItemTextList.at(i)
                << "#" << ui->tableWidget_temp_2->item(i, 3)->text();
        }
        break;
    }
    case IEC60884:
    {
        QString t2 = ui->lineEdit_tempMaxValue2->text();
        if (t2.isEmpty()) {
            t2 = "1";
        }
        out << ui->checkBox_temp->isChecked()
            << '\n' << ui->lineEdit_tempVn->text()
            << '\n' << ui->lineEdit_tempIn->text()
            << '\n' << ui->lineEdit_tempH->text()
            << '\n' << ui->checkBox_tempSwitch->isChecked()
            << '\n' << ui->checkBox_tempIC->isChecked()
            << '\n' << ui->lineEdit_tempIm->text()
            << '\n' << mAttachmentType
            << '\n' << ui->tableWidget_3->item(mAttachmentType,1)->text()
            << '\n' << ui->lineEdit_tempVn_t->text()
            << '\n' << ui->lineEdit_tempIn_t->text()
            << '\n' << t2;
        for (int i = 0; i<20; i++)
        {
            mItemTextList << ui->tableWidget_temp->item(i, 1)->text();
            out << '\n' << this->isChecked(ui->tableWidget_temp->item(i, 1))
                << mItemTextList.at(i)
                << "#" << ui->tableWidget_temp->item(i, 3)->text();
        }
        break;
    }
    }
}

void FormDisplay::clause20_out(QIODevice *device)
{
    QTextStream out(device);
    switch (mPlug) {
    case IEC60320:
    {
        out << ui->checkBox_breaking_2->isChecked()
            << '\n' << ui->lineEdit_breakingVn_2->text()
            << '\n' << ui->lineEdit_breakingIn_2->text()
            << '\n' << ui->lineEdit_breaking_rate_t_2->text()
            << '\n' << ui->lineEdit_breaking_position_t_2->text()
            << '\n' << ui->lineEdit_breakingVn_t_2->text()
            << '\n' << ui->lineEdit_breakingIn_t_2->text()
            << '\n' << ui->lineEdit_breaking_handleNum_2->text()
            << '\n' << ui->lineEdit_breaking_handleRate_2->text()
            << '\n' << ui->lineEdit_breaking_tongTime_2->text();
        break;
    }
    case IEC60884:
    {
        out << ui->checkBox_breaking->isChecked()
            << '\n' << ui->lineEdit_breakingVn->text()
            << '\n' << ui->lineEdit_breakingIn->text()
            << '\n' << ui->lineEdit_breaking_rate_t->text()
            << '\n' << ui->lineEdit_breaking_position_t->text()
            << '\n' << ui->lineEdit_breakingVn_t->text()
            << '\n' << ui->lineEdit_breakingIn_t->text()
            << '\n' << ui->lineEdit_breaking_handleNum->text()
            << '\n' << ui->lineEdit_breaking_handleRate->text()
            << '\n' << ui->lineEdit_breaking_tongTime->text();
        break;
    }
    }
}

void FormDisplay::clause21_out(QIODevice *device)
{
    QTextStream out(device);
    switch (mPlug) {
    case IEC60320:
    {
        out << ui->checkBox_normal_2->isChecked()
            << '\n' << ui->lineEdit_normalVn_2->text()
            << '\n' << ui->lineEdit_normalIn_2->text()
            << '\n' << ui->lineEdit_normal_rate_t_2->text()
            << '\n' << ui->lineEdit_normal_position_t_2->text()
            << '\n' << ui->lineEdit_normalVn_t_2->text()
            << '\n' << ui->lineEdit_normalIn_t_2->text()
            << '\n' << ui->lineEdit_normal_handleNum_2->text()
            << '\n' << ui->lineEdit_normal_handleRate_2->text();
        break;
    }
    case IEC60884:
    {
        out << ui->checkBox_normal->isChecked()
            << '\n' << ui->lineEdit_normalVn->text()
            << '\n' << ui->lineEdit_normalIn->text()
            << '\n' << ui->lineEdit_normal_rate_t->text()
            << '\n' << ui->lineEdit_normal_position_t->text()
            << '\n' << mAttachmentType
            << '\n' << ui->tableWidget_4->item(mAttachmentType, 1)->text()
            << '\n' << ui->lineEdit_normalVn_t->text()
            << '\n' << ui->lineEdit_normalIn_t->text()
            << '\n' << ui->lineEdit_normal_handleNum->text()
            << '\n' << ui->lineEdit_normal_handleRate->text()
            << '\n' << ui->lineEdit_normal_tongTime->text();
        break;
    }
    }
}

void FormDisplay::clause22_out(QIODevice *device)
{
    QTextStream out(device);
    QString t2 = ui->lineEdit_normalMaxValue2->text();
    if (t2.isEmpty()) {
        t2 = "1";
    }
    out << ui->checkBox_normalTemp->isChecked()
        << '\n' << ui->lineEdit_normalTVn->text()
        << '\n' << ui->lineEdit_normalTIn->text()
        << '\n' << ui->lineEdit_normalTH->text()
        << '\n' << ui->checkBox_normalTSwitch->isChecked()
        << '\n' << mAttachmentType
        << '\n' << ui->tableWidget_5->item(mAttachmentType, 1)->text()
        << '\n' << ui->lineEdit_normalTVn_t->text()
        << '\n' << ui->lineEdit_normalTIn_t->text()
        << '\n' << t2;
    for (int i = 0; i<20; i++)
    {
        mItemTextList << ui->tableWidget_normalTemp->item(i, 1)->text();
        out << '\n' << this->isChecked(ui->tableWidget_normalTemp->item(i, 1))
            << mItemTextList.at(i)
            << "#" << ui->tableWidget_normalTemp->item(i, 3)->text();
    }
}

void FormDisplay::clause19_in(QIODevice *device)
{
    QTextStream in(device);
    switch (mPlug) {
    case IEC60320:
    {
        ui->checkBox_temp_2->setChecked(in.readLine().toInt());
        ui->lineEdit_tempVn_2->setText(in.readLine());
        ui->lineEdit_tempIn_2->setText(in.readLine());
        ui->lineEdit_tempH_2->setText(in.readLine());
        if (ui->checkBox_temp_2->isChecked()) {
            ui->lineEdit_tempVn_t_2->setText(in.readLine());
            ui->lineEdit_tempIn_t_2->setText(in.readLine());
        } else {
            in.readLine();
            in.readLine();
        }
        ui->lineEdit_tempMaxValue2_2->setText(in.readLine());

        for (int i = 0; i < 20; i++)
        {
            QString str = in.readLine();

            if (str.left(1).toInt())ui->tableWidget_temp_2->item(i, 1)->setCheckState(Qt::Checked);
            else ui->tableWidget_temp_2->item(i, 1)->setCheckState(Qt::Unchecked);

            ui->tableWidget_temp_2->item(i, 1)->setText(str.mid(1, str.indexOf("#")-1));
            ui->tableWidget_temp_2->item(i, 3)->setText(str.mid(str.indexOf("#")+1));
        }
        break;
    }
    case IEC60884:
    {
        ui->checkBox_temp->setChecked(in.readLine().toInt());
        ui->lineEdit_tempVn->setText(in.readLine());
        ui->lineEdit_tempIn->setText(in.readLine());
        ui->lineEdit_tempH->setText(in.readLine());
        ui->checkBox_tempSwitch->setChecked(in.readLine().toInt());
        ui->checkBox_tempIC->setChecked(in.readLine().toInt());
        ui->lineEdit_tempIm->setText(in.readLine());
        mAttachmentType = in.readLine().toInt();
        ui->tableWidget_3->item(mAttachmentType,0)->setCheckState(Qt::Checked);
        ui->tableWidget_3->item(mAttachmentType,1)->setText(in.readLine());
        if (ui->checkBox_temp->isChecked()) {
            ui->lineEdit_tempVn_t->setText(in.readLine());
            ui->lineEdit_tempIn_t->setText(in.readLine());
        } else {
            in.readLine();
            in.readLine();
        }
        ui->lineEdit_tempMaxValue2->setText(in.readLine());
        for (int i = 0; i < 20; i++)
        {
            QString str = in.readLine();

            if (str.left(1).toInt()) ui->tableWidget_temp->item(i, 1)->setCheckState(Qt::Checked);
            else ui->tableWidget_temp->item(i, 1)->setCheckState(Qt::Unchecked);

            ui->tableWidget_temp->item(i, 1)->setText(str.mid(1, str.indexOf("#")-1));
            ui->tableWidget_temp->item(i, 3)->setText(str.mid(str.indexOf("#")+1));
        }
        break;
    }
    }
}

void FormDisplay::clause20_in(QIODevice *device)
{
    QTextStream in(device);
    switch (mPlug) {
    case IEC60320:
    {
        ui->checkBox_breaking_2->setChecked(in.readLine().toInt());
        ui->lineEdit_breakingVn_2->setText(in.readLine());
        ui->lineEdit_breakingIn_2->setText(in.readLine());
        ui->lineEdit_breaking_rate_t_2->setText(in.readLine());
        ui->lineEdit_breaking_position_t_2->setText(in.readLine());
        if (ui->checkBox_breaking_2->isChecked()) {
            ui->lineEdit_breakingVn_t_2->setText(in.readLine());
            ui->lineEdit_breakingIn_t_2->setText(in.readLine());
            ui->lineEdit_breaking_handleNum_2->setText(in.readLine());
        }
        break;
    }
    case IEC60884:
    {
        ui->checkBox_breaking->setChecked(in.readLine().toInt());
        ui->lineEdit_breakingVn->setText(in.readLine());
        ui->lineEdit_breakingIn->setText(in.readLine());
        ui->lineEdit_breaking_rate_t->setText(in.readLine());
        ui->lineEdit_breaking_position_t->setText(in.readLine());
        if (ui->checkBox_breaking->isChecked()) {
            ui->lineEdit_breakingVn_t->setText(in.readLine());
            ui->lineEdit_breakingIn_t->setText(in.readLine());
            ui->lineEdit_breaking_handleNum->setText(in.readLine());
        }
        break;
    }
    }
}

void FormDisplay::clause21_in(QIODevice *device)
{
    QTextStream in(device);
    switch (mPlug) {
    case IEC60320:
    {
        ui->checkBox_normal_2->setChecked(in.readLine().toInt());
        ui->lineEdit_normalVn_2->setText(in.readLine());
        ui->lineEdit_normalIn_2->setText(in.readLine());
        ui->lineEdit_normal_rate_t_2->setText(in.readLine());
        ui->lineEdit_normal_position_t_2->setText(in.readLine());
        if (ui->checkBox_normal_2->isChecked()) {
            ui->lineEdit_normalVn_t_2->setText(in.readLine());
            ui->lineEdit_normalIn_t_2->setText(in.readLine());
            ui->lineEdit_normal_handleNum_2->setText(in.readLine());
        }
        break;
    }
    case IEC60884:
    {
        ui->checkBox_normal->setChecked(in.readLine().toInt());
        ui->lineEdit_normalVn->setText(in.readLine());
        ui->lineEdit_normalIn->setText(in.readLine());
        ui->lineEdit_normal_rate_t->setText(in.readLine());
        ui->lineEdit_normal_position_t->setText(in.readLine());
        mAttachmentType = in.readLine().toInt();
        ui->tableWidget_4->item(mAttachmentType,0)->setCheckState(Qt::Checked);
        ui->tableWidget_4->item(mAttachmentType, 1)->setText(in.readLine());
        if (ui->checkBox_normal->isChecked()) {
            ui->lineEdit_normalVn_t->setText(in.readLine());
            ui->lineEdit_normalIn_t->setText(in.readLine());
            ui->lineEdit_normal_handleNum->setText(in.readLine());
        }
        break;
    }
    }
}

void FormDisplay::clause22_in(QIODevice *device)
{
    QTextStream in(device);
    ui->checkBox_normalTemp->setChecked(in.readLine().toInt());
    ui->lineEdit_normalTVn->setText(in.readLine());
    ui->lineEdit_normalTIn->setText(in.readLine());
    ui->lineEdit_normalTH->setText(in.readLine());
    ui->checkBox_normalTSwitch->setChecked(in.readLine().toInt());
    mAttachmentType = in.readLine().toInt();
    ui->tableWidget_5->item(mAttachmentType,0)->setCheckState(Qt::Checked);
    ui->tableWidget_5->item(mAttachmentType,1)->setText(in.readLine());
    if (ui->checkBox_normalTemp->isChecked()) {
        ui->lineEdit_normalTVn_t->setText(in.readLine());
        ui->lineEdit_normalTIn_t->setText(in.readLine());
    } else {
        in.readLine();
        in.readLine();
    }
    ui->lineEdit_normalMaxValue2->setText(in.readLine());

    for (int i = 0; i < 20; i++)
    {
        QString str = in.readLine();

        if (str.left(1).toInt()) {
            ui->tableWidget_normalTemp->item(i, 1)->setCheckState(Qt::Checked);
        } else {
            ui->tableWidget_normalTemp->item(i, 1)->setCheckState(Qt::Unchecked);
        }

        ui->tableWidget_normalTemp->item(i, 1)->setText(str.mid(1, str.indexOf("#")-1));
        ui->tableWidget_normalTemp->item(i, 3)->setText(str.mid(str.indexOf("#")+1));
    }
}

void FormDisplay::clause19_clear()
{
    ui->checkBox_temp->setChecked(0);
    ui->lineEdit_tempVn->clear();
    ui->lineEdit_tempIn->clear();
    ui->lineEdit_tempH->clear();
    ui->checkBox_tempSwitch->setChecked(0);
    ui->checkBox_tempIC->setChecked(0);
    mAttachmentType = 0;
    ui->tableWidget_3->item(0,0)->setCheckState(Qt::Checked);
    ui->tableWidget_temp->show();
    ui->tableWidget_temp->scrollToTop();
    ui->lineEdit_tempMaxValue2->setText("1");

    for (int i = 0; i < 20; i++)
    {
        if (i > 0) {
            ui->tableWidget_temp->item(i, 1)->setCheckState(Qt::Unchecked);
            ui->tableWidget_temp->item(i, 1)->setText("");
        }
        ui->tableWidget_temp->item(i, 3)->setText("");
    }
}

void FormDisplay::clause20_clear()
{
    ui->checkBox_breaking->setChecked(0);
    ui->lineEdit_breakingVn->clear();
    ui->lineEdit_breakingIn->clear();
    ui->lineEdit_breaking_rate_t->clear();
    ui->lineEdit_breaking_position_t->clear();
    ui->lineEdit_breaking_handleNum->setText("100");
    ui->lineEdit_breaking_handleRate->clear();
}

void FormDisplay::clause21_clear()
{
    ui->checkBox_normal->setChecked(0);
    ui->lineEdit_normalVn->clear();
    ui->lineEdit_normalIn->clear();
    ui->lineEdit_normal_rate_t->clear();
    ui->lineEdit_normal_position_t->clear();
    mAttachmentType = 0;
    ui->tableWidget_4->item(0,0)->setCheckState(Qt::Checked);
    ui->lineEdit_normal_handleNum->setText("10000");
    ui->lineEdit_normal_handleRate->clear();
}

void FormDisplay::clause22_clear()
{
    ui->checkBox_normalTemp->setChecked(0);
    ui->lineEdit_normalTVn->clear();
    ui->lineEdit_normalTIn->clear();
    ui->lineEdit_normalTH->clear();
    ui->checkBox_normalTSwitch->setChecked(0);
    mAttachmentType = 0;
    ui->tableWidget_5->item(0,0)->setCheckState(Qt::Checked);
    ui->tableWidget_normalTemp->show();
    ui->tableWidget_normalTemp->scrollToTop();
    ui->lineEdit_normalMaxValue2->setText("1");

    for (int i = 0; i < 20; i++)
    {
        if (i > 0) {
            ui->tableWidget_normalTemp->item(i, 1)->setCheckState(Qt::Unchecked);
            ui->tableWidget_normalTemp->item(i, 1)->setText("");
        }
        ui->tableWidget_normalTemp->item(i, 3)->setText("");
    }
}

void FormDisplay::clause19_2_clear()
{
    ui->checkBox_temp_2->setChecked(0);
    ui->lineEdit_tempVn_2->clear();
    ui->lineEdit_tempIn_2->clear();
    ui->lineEdit_tempH_2->clear();
    ui->tableWidget_temp_2->show();
    ui->tableWidget_temp_2->scrollToTop();
    ui->lineEdit_tempMaxValue2_2->setText("1");

    for (int i = 0; i < 20; i++)
    {
        if (i > 0) {
            ui->tableWidget_temp_2->item(i, 1)->setCheckState(Qt::Unchecked);
            ui->tableWidget_temp_2->item(i, 1)->setText("");
        }
        ui->tableWidget_temp_2->item(i, 3)->setText("");
    }
}

void FormDisplay::clause20_2_clear()
{
    ui->checkBox_breaking_2->setChecked(0);
    ui->lineEdit_breakingVn_2->clear();
    ui->lineEdit_breakingIn_2->clear();
    ui->lineEdit_breaking_rate_t_2->clear();
    ui->lineEdit_breaking_position_t_2->clear();
    ui->lineEdit_breaking_handleNum_2->setText("100");
}

void FormDisplay::clause21_2_clear()
{
    ui->checkBox_normal_2->setChecked(0);
    ui->lineEdit_normalVn_2->clear();
    ui->lineEdit_normalIn_2->clear();
    ui->lineEdit_normal_rate_t_2->clear();
    ui->lineEdit_normal_position_t_2->clear();
    ui->lineEdit_normal_handleNum_2->setText("100");
}

void FormDisplay::IEC320_clear()
{
    ui->lineEditVn_2->clear();
    ui->lineEditIn_2->clear();
    ui->lineEditH_2->clear();
    ui->lineEditRate_2->clear();
    ui->lineEditPosition_2->clear();
    ui->lineEditMaxValue_2->setText("1");

    for (int i = 0; i < 20; i++)
    {
        if (i > 0) {
            ui->tableWidget_2->item(i, 1)->setCheckState(Qt::Unchecked);
            ui->tableWidget_2->item(i, 1)->setText("");
        }
        ui->tableWidget_2->item(i, 3)->setText("");
    }
    ui->tableWidget_2->scrollToTop();

    ui->lineEditVn_t_5->clear();
    ui->lineEditVn_t_6->clear();
    ui->lineEditVn_t_7->clear();
    ui->lineEditIn_t_5->clear();
    ui->lineEditIn_t_6->clear();
    ui->lineEditIn_t_7->clear();
    ui->lineEditHandleNum_5->clear();

    ui->lineEditRn_6->setText(tr("Inductive Load(0.6)"));
    ui->lineEditRn_7->setText(tr("Inductive Load(0.6)"));
}

void FormDisplay::IEC884_clear()
{
    ui->lineEditVn->clear();
    ui->lineEditIn->clear();
    ui->lineEditH->clear();
    ui->radioButtonSwitch->setChecked(0);
    ui->radioButtonIC->setChecked(0);
    mAttachmentType = 0;
    ui->tableWidget_6->item(0,0)->setCheckState(Qt::Checked);
    ui->lineEditRate->clear();
    ui->lineEditPosition->clear();
    ui->lineEditMaxValue->setText("1");

    for (int i = 0; i < 20; i++)
    {
        if (i > 0) {
            ui->tableWidget->item(i, 1)->setCheckState(Qt::Unchecked);
            ui->tableWidget->item(i, 1)->setText("");
        }
        ui->tableWidget->item(i, 3)->setText("");
    }
    ui->tableWidget->scrollToTop();

    ui->lineEditVn_t_1->clear();
    ui->lineEditVn_t_2->clear();
    ui->lineEditVn_t_3->clear();
    ui->lineEditVn_t_4->clear();
    ui->lineEditIn_t_1->clear();
    ui->lineEditIn_t_2->clear();
    ui->lineEditIn_t_3->clear();
    ui->lineEditIn_t_4->clear();
    ui->lineEditHandleRate_1->clear();
    ui->lineEditHandleRate_2->clear();
    ui->lineEditTongTime_1->clear();
    ui->lineEditTongTime_2->clear();
}

void FormDisplay::clickedEnglishAction()
{
    QSettings *configIniWrite = new QSettings("EATconfig.ini", QSettings::IniFormat);
    configIniWrite->setValue("Language", 0);
    delete configIniWrite;

    QMessageBox msgBox(QMessageBox::Information,tr("Language"),tr("The language change will take effect after a restart of program."));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
    msgBox.setWindowIcon(QIcon(WINDOW_ICON));

    msgBox.exec();
}

void FormDisplay::clickedChineseAction()
{
    QSettings *configIniWrite = new QSettings("EATconfig.ini", QSettings::IniFormat);
    configIniWrite->setValue("Language", 1);
    delete configIniWrite;

    QMessageBox msgBox(QMessageBox::Information,tr("Language"),tr("The language change will take effect after a restart of program."));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
    msgBox.setWindowIcon(QIcon(WINDOW_ICON));

    msgBox.exec();
}

void FormDisplay::on_toolButton_save19_clicked()
{
    //    mItemTextList << ui->tableWidget_temp_2->item(i, 0)->text();
    //    out << '\n' << this->isChecked(ui->tableWidget_temp_2->item(i, 0))
    //        << mItemTextList.at(i);
    if (!this->isChecked(ui->tableWidget_temp->item(ui->lineEdit_tempMaxValue2->text().toInt()-1, 0))) {
        ui->tableWidget_temp->item(ui->lineEdit_tempMaxValue2->text().toInt()-1, 0)->setCheckState(Qt::Checked);
    }

    if (ui->tableWidget_temp->item(ui->lineEdit_tempMaxValue2->text().toInt()-1, 0)->text().toInt() != 25) {
        ui->tableWidget_temp->item(ui->lineEdit_tempMaxValue2->text().toInt()-1, 0)->setText("25");
    }

    addItemIcon(IMAGE_CLAUSE19, CLAUSE_19);
}

void FormDisplay::on_toolButton_save20_clicked()
{
    addItemIcon(IMAGE_CLAUSE20, CLAUSE_20);
}

void FormDisplay::on_toolButton_save21_clicked()
{
    addItemIcon(IMAGE_CLAUSE21, CLAUSE_21);
}

void FormDisplay::on_toolButton_save22_clicked()
{
    if (!this->isChecked(ui->tableWidget_normalTemp->item(ui->lineEdit_normalMaxValue2->text().toInt()-1, 0))) {
        ui->tableWidget_normalTemp->item(ui->lineEdit_normalMaxValue2->text().toInt()-1, 0)->setCheckState(Qt::Checked);
    }

    if (ui->tableWidget_normalTemp->item(ui->lineEdit_normalMaxValue2->text().toInt()-1, 0)->text().toInt() != 25) {
        ui->tableWidget_normalTemp->item(ui->lineEdit_normalMaxValue2->text().toInt()-1, 0)->setText("25");
    }

    addItemIcon(IMAGE_CLAUSE22, CLAUSE_22);
}

void FormDisplay::on_pushButton_login1_clicked()
{
    mPlug = IEC60320;
    enterMain();
}

void FormDisplay::on_pushButton_login2_clicked()
{
    mPlug = IEC60884;
    enterMain();
}

void FormDisplay::on_toolButton_19_clicked()
{
    switch (mPlug) {
    case IEC60320:
    {
        clause19_2_clear();
        ui->stackedWidget->setCurrentIndex(STACKWIDGET_CLAUSE19_320);
        break;
    }
    case IEC60884:
    {
        clause19_clear();
        ui->stackedWidget->setCurrentIndex(STACKWIDGET_CLAUSE19_884);
        break;
    }
    }
}

void FormDisplay::on_toolButton_20_clicked()
{
    switch (mPlug) {
    case IEC60320:
    {
        clause20_2_clear();
        ui->stackedWidget->setCurrentIndex(STACKWIDGET_CLAUSE20_320);
        break;
    }
    case IEC60884:
    {
        clause20_clear();
        ui->stackedWidget->setCurrentIndex(STACKWIDGET_CLAUSE20_884);
        break;
    }
    }
}

void FormDisplay::on_toolButton_21_clicked()
{
    switch (mPlug) {
    case IEC60320:
    {
        clause21_2_clear();
        ui->stackedWidget->setCurrentIndex(STACKWIDGET_CLAUSE21_320);
        break;
    }
    case IEC60884:
    {
        clause21_clear();
        ui->stackedWidget->setCurrentIndex(STACKWIDGET_CLAUSE21_884);
        break;
    }
    }
}

void FormDisplay::on_toolButton_22_clicked()
{
    clause22_clear();
    ui->stackedWidget->setCurrentIndex(STACKWIDGET_CLAUSE22_884);
}

void FormDisplay::on_toolButton_close_clicked()
{
    if (!ui->toolButton_start->isEnabled()||!ui->toolButton_start_2->isEnabled()||!ui->toolButton_start_3->isEnabled()) {
        QMessageBox msgBox(QMessageBox::Warning,tr("Error"),tr("The project is testing.\nPlease be patient."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
        msgBox.setWindowIcon(QIcon(WINDOW_ICON));

        msgBox.exec();
        return;
    }

    QMessageBox msgBox(QMessageBox::Question,tr("Exit"),tr("Whether out of the project?"));
    msgBox.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
    msgBox.setButtonText(QMessageBox::Cancel,tr("Cancel"));
    msgBox.setWindowIcon(QIcon(WINDOW_ICON));

    int ret = msgBox.exec();

    if (ret == QMessageBox::Cancel) {
        return;
    }

    mMessage->show();
    mMessage->setText(" ");

    ui->lineEdit_itemId->clear();
    mFileNameListA.clear();
    mFileNameListB.clear();
    mFileNameListC.clear();
    mPiecesViewListA->close();
    mPiecesPowerListA->close();
    mPiecesViewListB->close();
    mPiecesPowerListB->close();
    mPiecesViewListC->close();
    mPiecesPowerListC->close();
    ui->stackedWidget->setCurrentIndex(STACKWIDGET_LOGIN);

    dirName = dirName.left(fileSize);
}

void FormDisplay::onCloseEditClicked()
{
    QMessageBox msgBox(QMessageBox::Question,tr("Quit"),tr("Whether out of the editor?"));
    msgBox.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
    msgBox.setButtonText(QMessageBox::Cancel,tr("Cancel"));
    msgBox.setWindowIcon(QIcon(WINDOW_ICON));

    int ret = msgBox.exec();

    if (ret == QMessageBox::Cancel) {
        return;
    }

    isOld = PROJECT_CONDITION_NEW;
    ui->stackedWidget->setCurrentIndex(STACKWIDGET_MAIN);
}

void FormDisplay::on_toolButton_start_clicked()
{
    if (!mPiecesPowerListA->count()) {
        mMessage->show();
        mMessage->setText(tr("Please Add Item!"));
        return;
    }

    if (!connectSTAS) {
        QMessageBox msgBox(QMessageBox::Warning,tr("Error"),tr("Connect STAS False!\nPlease after the restart to try again."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
        msgBox.setWindowIcon(QIcon(WINDOW_ICON));

        msgBox.exec();
        return;
    }

    if (!ui->checkBox_VOLT->isChecked()) {
        QMessageBox msgBox(QMessageBox::Warning,tr("Error"),tr("Power Supply UART was disabled!\nPlease check and update the UART Set."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
        msgBox.setWindowIcon(QIcon(WINDOW_ICON));

        msgBox.exec();
        return;
    }/* else {
        QString volt = CollectControl::HardSend(devInformation.at(0).com, QString("RDW VF").toLatin1().data(), 100);
        if (volt.isEmpty() || volt.size() < 14) {
            qDebug() << volt.size();
            devInformation.removeAt(0);
            UART_t voltUart;
            voltUart.data = volt;
            voltUart.com = comVolt;
            devInformation.insert(0,voltUart);
            ui->checkBox_VOLT->setChecked(false);
        }
    }*/

    if (!ui->checkBox_A->isChecked()) {
        QMessageBox msgBox(QMessageBox::Warning,tr("Error"),tr("Load(#4) UART was disabled!\nPlease check and update the UART Set."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
        msgBox.setWindowIcon(QIcon(WINDOW_ICON));

        msgBox.exec();
        return;
    }

    dirNameListA.clear();

    for (int i = 0; i < mPiecesPowerListA->count(); i++)
    {
        QString fileName = mPiecesPowerListA->item(i)->text();
        if (fileName.right(5).mid(0,2).toInt() == 19 || fileName.right(5).mid(0,2).toInt() == 22) {
            if (!ui->checkBox_TRC->isChecked()) {
                QMessageBox msgBox(QMessageBox::Warning,tr("Error"),tr("Paperless Recorder UART was disabled!\nPlease check and update the UART Set."));
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
                msgBox.setWindowIcon(QIcon(WINDOW_ICON));

                msgBox.exec();
                return;
            }
        } else if (fileName.right(5).mid(0,2).toInt() == 20 || fileName.right(5).mid(0,2).toInt() == 21) {
            if (!ui->checkBox_TEST->isChecked()) {
                QMessageBox msgBox(QMessageBox::Warning,tr("Error"),tr("Life Tester UART was disabled!\nPlease check and update the UART Set."));
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
                msgBox.setWindowIcon(QIcon(WINDOW_ICON));

                msgBox.exec();
                return;
            }
        }
        dirNameListA << dirName+fileName;
    }

    ui->toolButton_start->setEnabled(false);
    mPiecesPowerListA->setAcceptDrops(false);
    mPiecesPowerListA->setDragEnabled(false);
    emit isSectionFileName(dirNameListA.at(0), mPlug);
}

void FormDisplay::on_lineEdit_tempIn_textChanged(const QString &arg1)
{
    if (!ui->checkBox_temp->isChecked()) {
        if (arg1 == RATED_CURRENT_2_5A) {
            if (ui->lineEdit_tempVn->text() >= RARED_VOLTAGE_130V &&
                    ui->lineEdit_tempVn->text() <= RARED_VOLTAGE_250V) {
                if (mAttachmentType == NON_REWIRABLE_PLUG) {
                    if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "Tinsel") ui->lineEdit_tempIn_t->setText("1");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "0.5") ui->lineEdit_tempIn_t->setText("2.5");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "0.75") ui->lineEdit_tempIn_t->setText("4");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_tempIn_t->setText("4");
                    else  ui->lineEdit_tempIn_t->clear();
                } else ui->lineEdit_tempIn_t->clear();
            } else  ui->lineEdit_tempIn_t->clear();
        }else if (arg1 == RATED_CURRENT_6A) {
            if (ui->lineEdit_tempVn->text() >= RARED_VOLTAGE_130V &&
                    ui->lineEdit_tempVn->text() <= RARED_VOLTAGE_250V) {
                if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_tempIn_t->setText("9");
                else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_tempIn_t->setText("8.4");
                else if (mAttachmentType == NON_REWIRABLE_PLUG) {
                    if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "Tinsel") ui->lineEdit_tempIn_t->setText("1");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "0.5") ui->lineEdit_tempIn_t->setText("2.5");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "0.75") ui->lineEdit_tempIn_t->setText("9");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_tempIn_t->setText("9");
                    else  ui->lineEdit_tempIn_t->clear();
                } else ui->lineEdit_tempIn_t->clear();
            } else ui->lineEdit_tempIn_t->clear();
        } else if (arg1 == RATED_CURRENT_10A) {
            if (ui->lineEdit_tempVn->text() >= RARED_VOLTAGE_130V &&
                    ui->lineEdit_tempVn->text() <= RARED_VOLTAGE_250V) {
                if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_tempIn_t->setText("16");
                else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_tempIn_t->setText("14");
                else if (mAttachmentType == NON_REWIRABLE_PORTABLE_SOCKET_OUTLET) {
                    if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "0.75") ui->lineEdit_tempIn_t->setText("10");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_tempIn_t->setText("12");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_tempIn_t->setText("16");
                    else  ui->lineEdit_tempIn_t->clear();
                } else if (mAttachmentType == NON_REWIRABLE_PLUG) {
                    if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "0.5") ui->lineEdit_tempIn_t->setText("2.5");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "0.75") ui->lineEdit_tempIn_t->setText("10");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_tempIn_t->setText("12");
                    else  ui->lineEdit_tempIn_t->clear();
                } else  ui->lineEdit_tempIn_t->clear();
            } else if (ui->lineEdit_tempVn->text() == RARED_VOLTAGE_440V) {
                if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_tempIn_t->setText("16");
                else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_tempIn_t->setText("14");
                else if (mAttachmentType == NON_REWIRABLE_PORTABLE_SOCKET_OUTLET) {
                    if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_tempIn_t->setText("12");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_tempIn_t->setText("16");
                    else  ui->lineEdit_tempIn_t->clear();
                } else if (mAttachmentType == NON_REWIRABLE_PLUG) {
                    if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_tempIn_t->setText("12");
                    else  ui->lineEdit_tempIn_t->clear();
                } else  ui->lineEdit_tempIn_t->clear();
            } else  ui->lineEdit_tempIn_t->clear();
        } else if (arg1 == RATED_CURRENT_13A) {
            if (ui->lineEdit_tempVn->text() == RARED_VOLTAGE_250V) {
                if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_tempIn_t->setText("17");
                else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_tempIn_t->setText("17");
                else if (mAttachmentType == NON_REWIRABLE_PORTABLE_SOCKET_OUTLET) {
                    if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "0.75") ui->lineEdit_tempIn_t->setText("10");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_tempIn_t->setText("12");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_tempIn_t->setText("13");
                    else  ui->lineEdit_tempIn_t->clear();
                } else if (mAttachmentType == NON_REWIRABLE_PLUG) {
                    if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "0.5") ui->lineEdit_tempIn_t->setText("2.5");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "0.75") ui->lineEdit_tempIn_t->setText("10");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_tempIn_t->setText("12");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_tempIn_t->setText("13");
                    else  ui->lineEdit_tempIn_t->clear();
                } else  ui->lineEdit_tempIn_t->clear();
            } else  ui->lineEdit_tempIn_t->clear();
        } else if (arg1 == RATED_CURRENT_16A) {
            if (ui->lineEdit_tempVn->text() >= RARED_VOLTAGE_130V &&
                    ui->lineEdit_tempVn->text() <= RARED_VOLTAGE_250V) {
                if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_tempIn_t->setText("22");
                else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_tempIn_t->setText("20");
                else if (mAttachmentType == NON_REWIRABLE_PORTABLE_SOCKET_OUTLET) {
                    if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_tempIn_t->setText("16");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_tempIn_t->setText("16");
                    else  ui->lineEdit_tempIn_t->clear();
                }else if (mAttachmentType == NON_REWIRABLE_PLUG) {
                    if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "Tinsel") ui->lineEdit_tempIn_t->setText("1");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "0.5") ui->lineEdit_tempIn_t->setText("2.5");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "0.75") ui->lineEdit_tempIn_t->setText("10");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_tempIn_t->setText("12");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_tempIn_t->setText("16");
                    else  ui->lineEdit_tempIn_t->clear();
                } else  ui->lineEdit_tempIn_t->clear();
            } else if (ui->lineEdit_tempVn->text() == RARED_VOLTAGE_440V) {
                if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_tempIn_t->setText("22");
                else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_tempIn_t->setText("20");
                else if (mAttachmentType == NON_REWIRABLE_PORTABLE_SOCKET_OUTLET) {
                    if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_tempIn_t->setText("16");
                    else  ui->lineEdit_tempIn_t->clear();
                } else if (mAttachmentType == NON_REWIRABLE_PLUG) {
                    if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_tempIn_t->setText("16");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "2.5") ui->lineEdit_tempIn_t->setText("22");
                    else  ui->lineEdit_tempIn_t->clear();
                } else  ui->lineEdit_tempIn_t->clear();
            } else  ui->lineEdit_tempIn_t->clear();
        } else if (arg1 == RATED_CURRENT_25A) {
            if (ui->lineEdit_tempVn->text() == RARED_VOLTAGE_250V) {
                if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_tempIn_t->setText("6");
                else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_tempIn_t->setText("25");
                else if (mAttachmentType == NON_REWIRABLE_PORTABLE_SOCKET_OUTLET) {
                    if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "32") ui->lineEdit_tempIn_t->setText("25");
                    else  ui->lineEdit_tempIn_t->clear();
                } else  ui->lineEdit_tempIn_t->clear();
            } else  ui->lineEdit_tempIn_t->clear();
        } else if (arg1 == RATED_CURRENT_32A) {
            if (ui->lineEdit_tempVn->text() >= RARED_VOLTAGE_130V &&
                    ui->lineEdit_tempVn->text() <= RARED_VOLTAGE_440V) {
                if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_tempIn_t->setText("40");
                else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_tempIn_t->setText("40");
                else if (mAttachmentType == NON_REWIRABLE_PORTABLE_SOCKET_OUTLET) {
                    if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "2.5") ui->lineEdit_tempIn_t->setText("25");
                    else  ui->lineEdit_tempIn_t->clear();
                } else if (mAttachmentType == NON_REWIRABLE_PLUG) {
                    if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "2.5") ui->lineEdit_tempIn_t->setText("25");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "4") ui->lineEdit_tempIn_t->setText("31");
                    else if (ui->tableWidget_3->item(mAttachmentType, 1)->text() == "6") ui->lineEdit_tempIn_t->setText("42");
                    else  ui->lineEdit_tempIn_t->clear();
                } else  ui->lineEdit_tempIn_t->clear();
            } else  ui->lineEdit_tempIn_t->clear();
        } else ui->lineEdit_tempIn_t->clear();
    }
}

void FormDisplay::on_lineEdit_tempVn_textChanged(const QString &arg1)
{
    if (!ui->checkBox_temp->isChecked()) {
        if (!arg1.isEmpty()) {
            ui->lineEdit_tempVn_t->setText(arg1);
        } else {
            ui->lineEdit_tempVn_t->clear();
        }
        on_lineEdit_tempIn_textChanged(ui->lineEdit_tempIn->text());
    }
}

void FormDisplay::on_lineEdit_tempIn_t_textChanged(const QString &arg1)
{
    if (ui->stackedWidget->currentIndex() == STACKWIDGET_884AUTO) {
        ui->lineEditIn_t_1->setText(arg1);
    }

    if (!arg1.isEmpty() &&
            !ui->lineEdit_tempVn_t->text().isEmpty() &&
            !ui->lineEdit_tempH->text().isEmpty()){
        if (ui->checkBox_tempIC->isChecked()) {
            if (!ui->lineEdit_tempIm->text().isEmpty() &&
                    ui->lineEdit_tempIm->text().toInt() > 0) {
                ui->toolButton_save19->setEnabled(true);
                if (ui->stackedWidget->currentIndex() == 9 &&
                        ui->toolButton_save20->isEnabled() &&
                        ui->toolButton_save21->isEnabled() &&
                        ui->toolButton_save22->isEnabled()) {
                    ui->toolButtonSave->setEnabled(true);
                } else {
                    ui->toolButtonSave->setEnabled(false);
                }
            } else ui->toolButton_save19->setEnabled(false);
        } else {
            ui->toolButton_save19->setEnabled(true);
            if (ui->stackedWidget->currentIndex() == 9 &&
                    ui->toolButton_save20->isEnabled() &&
                    ui->toolButton_save21->isEnabled() &&
                    ui->toolButton_save22->isEnabled()) {
                ui->toolButtonSave->setEnabled(true);
            } else {
                ui->toolButtonSave->setEnabled(false);
            }
        }
    } else ui->toolButton_save19->setEnabled(false);
}

void FormDisplay::on_lineEdit_tempH_textChanged(const QString &arg1)
{
    if (!arg1.isEmpty() &&
            !ui->lineEdit_tempVn_t->text().isEmpty() &&
            !ui->lineEdit_tempIn_t->text().isEmpty()){
        if (ui->checkBox_tempIC->isChecked()) {
            if (!ui->lineEdit_tempIm->text().isEmpty() &&
                    ui->lineEdit_tempIm->text().toInt() > 0) {
                ui->toolButton_save19->setEnabled(true);
                if (ui->stackedWidget->currentIndex() == 9 &&
                        ui->toolButton_save20->isEnabled() &&
                        ui->toolButton_save21->isEnabled() &&
                        ui->toolButton_save22->isEnabled()) {
                    ui->toolButtonSave->setEnabled(true);
                } else {
                    ui->toolButtonSave->setEnabled(false);
                }
            } else ui->toolButton_save19->setEnabled(false);
        } else  {
            ui->toolButton_save19->setEnabled(true);
            if (ui->stackedWidget->currentIndex() == 9 &&
                    ui->toolButton_save20->isEnabled() &&
                    ui->toolButton_save21->isEnabled() &&
                    ui->toolButton_save22->isEnabled()) {
                ui->toolButtonSave->setEnabled(true);
            } else {
                ui->toolButtonSave->setEnabled(false);
            }
        }
    } else ui->toolButton_save19->setEnabled(false);
}

void FormDisplay::on_lineEdit_tempIm_textChanged(const QString &arg1)
{
    if (!ui->lineEdit_tempIn_t->text().isEmpty() &&
            !ui->lineEdit_tempVn_t->text().isEmpty() &&
            !ui->lineEdit_tempH->text().isEmpty()){
        if (ui->checkBox_tempIC->isChecked()) {
            if (!arg1.isEmpty() && arg1.toInt() > 0) {
                ui->toolButton_save19->setEnabled(true);
                if (ui->stackedWidget->currentIndex() == 9 &&
                        ui->toolButton_save20->isEnabled() &&
                        ui->toolButton_save21->isEnabled() &&
                        ui->toolButton_save22->isEnabled()) {
                    ui->toolButtonSave->setEnabled(true);
                } else {
                    ui->toolButtonSave->setEnabled(false);
                }
            } else ui->toolButton_save19->setEnabled(false);
        } else {
            ui->toolButton_save19->setEnabled(true);
            if (ui->stackedWidget->currentIndex() == 9 &&
                    ui->toolButton_save20->isEnabled() &&
                    ui->toolButton_save21->isEnabled() &&
                    ui->toolButton_save22->isEnabled()) {
                ui->toolButtonSave->setEnabled(true);
            } else {
                ui->toolButtonSave->setEnabled(false);
            }
        }
    } else ui->toolButton_save19->setEnabled(false);
}

void FormDisplay::on_checkBox_tempIC_toggled(bool checked)
{
    if (checked) {
        ui->label_tempIm->show();
        ui->lineEdit_tempIm->show();
    } else {
        ui->label_tempIm->hide();
        ui->lineEdit_tempIm->hide();
        ui->lineEdit_tempIm->clear();
    }

    if (!ui->lineEdit_tempIn_t->text().isEmpty() &&
            !ui->lineEdit_tempVn_t->text().isEmpty() &&
            !ui->lineEdit_tempH->text().isEmpty()){
        if (checked) {
            if (!ui->lineEdit_tempIm->text().isEmpty() &&
                    ui->lineEdit_tempIm->text().toInt() > 0) {
                ui->toolButton_save19->setEnabled(true);
                if (ui->stackedWidget->currentIndex() == 9 &&
                        ui->toolButton_save20->isEnabled() &&
                        ui->toolButton_save21->isEnabled() &&
                        ui->toolButton_save22->isEnabled()) {
                    ui->toolButtonSave->setEnabled(true);
                } else {
                    ui->toolButtonSave->setEnabled(false);
                }
            } else ui->toolButton_save19->setEnabled(false);
        } else {
            ui->toolButton_save19->setEnabled(true);
            if (ui->stackedWidget->currentIndex() == 9 &&
                    ui->toolButton_save20->isEnabled() &&
                    ui->toolButton_save21->isEnabled() &&
                    ui->toolButton_save22->isEnabled()) {
                ui->toolButtonSave->setEnabled(true);
            } else {
                ui->toolButtonSave->setEnabled(false);
            }
        }
    } else ui->toolButton_save19->setEnabled(false);
}

void FormDisplay::on_lineEdit_breakingVn_textChanged(const QString &arg1)
{
    if (0 < arg1.toInt() && arg1.toInt() <= 250 &&
            0 < ui->lineEdit_breakingIn->text().toInt() && ui->lineEdit_breakingIn->text().toInt() <= 16){
        ui->lineEdit_breaking_handleRate->setText(HANDLE_RATE_30_MIN);
    } else{
        ui->lineEdit_breaking_handleRate->setText(HANDLE_RATE_15_MIN);
    }

    if (!ui->checkBox_breaking->isChecked()) {
        if (!arg1.isEmpty()){
            ui->lineEdit_breakingVn_t->setText(QString::number(arg1.toDouble() * 1.1));
        } else {
            ui->lineEdit_breakingVn_t->clear();
            ui->lineEdit_breaking_handleRate->clear();
        }
    }
}

void FormDisplay::on_lineEdit_breakingIn_textChanged(const QString &arg1)
{
    if (0 < arg1.toInt() && arg1.toInt() <= 16) {
        ui->lineEdit_breaking_tongTime->setText(HANDLE_TIME_1_5_SECOND);
        if (0 < ui->lineEdit_breakingVn->text().toInt() && ui->lineEdit_breakingVn->text().toInt() <= 250){
            ui->lineEdit_breaking_handleRate->setText(HANDLE_RATE_30_MIN);
        }
    } else {
        ui->lineEdit_breaking_tongTime->setText(HANDLE_TIME_3_0_SECOND);
        ui->lineEdit_breaking_handleRate->setText(HANDLE_RATE_15_MIN);
    }

    if (!ui->checkBox_breaking->isChecked()) {
        if (!arg1.isEmpty()){
            ui->lineEdit_breakingIn_t->setText(QString::number(arg1.toDouble()*1.25));
        } else {
            ui->lineEdit_breakingIn_t->clear();
            ui->lineEdit_breaking_tongTime->clear();
        }
    }
}

void FormDisplay::on_lineEdit_breakingVn_t_textChanged(const QString &arg1)
{
    if (ui->stackedWidget->currentIndex() == STACKWIDGET_884AUTO) {
        ui->lineEditVn_t_2->setText(arg1);
    }

    if (!arg1.isEmpty() && !ui->lineEdit_breakingIn_t->text().isEmpty() &&
            !ui->lineEdit_breaking_handleRate->text().isEmpty() &&
            !ui->lineEdit_breaking_rate_t->text().isEmpty() &&
            !ui->lineEdit_breaking_position_t->text().isEmpty()){
        ui->toolButton_save20->setEnabled(true);
        if (ui->stackedWidget->currentIndex() == 9 &&
                ui->toolButton_save19->isEnabled() &&
                ui->toolButton_save21->isEnabled() &&
                ui->toolButton_save22->isEnabled()) {
            ui->toolButtonSave->setEnabled(true);
        } else {
            ui->toolButtonSave->setEnabled(false);
        }
    } else {
        ui->toolButton_save20->setEnabled(false);
    }
}

void FormDisplay::on_lineEdit_breakingIn_t_textChanged(const QString &arg1)
{
    if (ui->stackedWidget->currentIndex() == STACKWIDGET_884AUTO) {
        ui->lineEditIn_t_2->setText(arg1);
    }

    if (!arg1.isEmpty() && !ui->lineEdit_breaking_tongTime->text().isEmpty() &&
            !ui->lineEdit_breaking_rate_t->text().isEmpty() &&
            !ui->lineEdit_breaking_position_t->text().isEmpty()){
        ui->toolButton_save20->setEnabled(true);
        if (ui->stackedWidget->currentIndex() == 9 &&
                ui->toolButton_save19->isEnabled() &&
                ui->toolButton_save21->isEnabled() &&
                ui->toolButton_save22->isEnabled()) {
            ui->toolButtonSave->setEnabled(true);
        } else {
            ui->toolButtonSave->setEnabled(false);
        }
    } else {
        ui->toolButton_save20->setEnabled(false);
    }
}

void FormDisplay::on_lineEdit_breaking_rate_t_textChanged(const QString &arg1)
{
    if (!arg1.isEmpty() && !ui->lineEdit_breaking_position_t->text().isEmpty() &&
            !ui->lineEdit_breakingIn_t->text().isEmpty() &&
            !ui->lineEdit_breaking_tongTime->text().isEmpty()){
        ui->toolButton_save20->setEnabled(true);
        if (ui->stackedWidget->currentIndex() == 9 &&
                ui->toolButton_save19->isEnabled() &&
                ui->toolButton_save21->isEnabled() &&
                ui->toolButton_save22->isEnabled()) {
            ui->toolButtonSave->setEnabled(true);
        } else {
            ui->toolButtonSave->setEnabled(false);
        }
    } else {
        ui->toolButton_save20->setEnabled(false);
    }
}

void FormDisplay::on_lineEdit_breaking_position_t_textChanged(const QString &arg1)
{
    if (!arg1.isEmpty() && !ui->lineEdit_breaking_rate_t->text().isEmpty() &&
            !ui->lineEdit_breakingIn_t->text().isEmpty() &&
            !ui->lineEdit_breaking_tongTime->text().isEmpty()){
        ui->toolButton_save20->setEnabled(true);
        if (ui->stackedWidget->currentIndex() == 9 &&
                ui->toolButton_save19->isEnabled() &&
                ui->toolButton_save21->isEnabled() &&
                ui->toolButton_save22->isEnabled()) {
            ui->toolButtonSave->setEnabled(true);
        } else {
            ui->toolButtonSave->setEnabled(false);
        }
    } else {
        ui->toolButton_save20->setEnabled(false);
    }
}

void FormDisplay::on_lineEdit_normalVn_textChanged(const QString &arg1)
{
    if (0 < arg1.toInt() && arg1.toInt() <= 250 &&
            0 < ui->lineEdit_normalIn->text().toInt() && ui->lineEdit_normalIn->text().toInt() <= 16){
        ui->lineEdit_normal_handleRate->setText(HANDLE_RATE_30_MIN);
    } else{
        ui->lineEdit_normal_handleRate->setText(HANDLE_RATE_15_MIN);
    }

    if (!ui->checkBox_normal->isChecked()) {
        if (!arg1.isEmpty()){
            ui->lineEdit_normalVn_t->setText(arg1);
        } else {
            ui->lineEdit_normalVn_t->clear();
            ui->lineEdit_normal_handleRate->clear();
        }

        on_lineEdit_normalIn_textChanged(ui->lineEdit_normalIn->text());
    }
}

void FormDisplay::on_lineEdit_normalIn_textChanged(const QString &arg1)
{
    if (0 < arg1.toInt() && arg1.toInt() <= 16) {
        ui->lineEdit_normal_tongTime->setText(HANDLE_TIME_1_5_SECOND);
        if (0 < ui->lineEdit_normalVn->text().toInt() && ui->lineEdit_normalVn->text().toInt() <= 250){
            ui->lineEdit_normal_handleRate->setText(HANDLE_RATE_30_MIN);
        }
    } else {
        ui->lineEdit_normal_tongTime->setText(HANDLE_TIME_3_0_SECOND);
        ui->lineEdit_normal_handleRate->setText(HANDLE_RATE_15_MIN);
    }


    if (!ui->checkBox_normal->isChecked()) {
        if (!arg1.isEmpty()) {
            if (arg1 == RATED_CURRENT_2_5A) {
                if (ui->lineEdit_normalVn->text() >= RARED_VOLTAGE_130V &&
                        ui->lineEdit_normalVn->text() <= RARED_VOLTAGE_250V) {
                    if (mAttachmentType == NON_REWIRABLE_PLUG) {
                        if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "Tinsel") ui->lineEdit_normalIn_t->setText("1");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "0.5") ui->lineEdit_normalIn_t->setText("2.5");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "0.75") ui->lineEdit_normalIn_t->setText("2.5");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_normalIn_t->setText("2.5");
                        else  ui->lineEdit_normalIn_t->clear();
                    } else  ui->lineEdit_normalIn_t->clear();
                } else  ui->lineEdit_normalIn_t->clear();
            } else if (arg1 == RATED_CURRENT_6A) {
                if (ui->lineEdit_normalVn->text() >= RARED_VOLTAGE_130V &&
                        ui->lineEdit_normalVn->text() <= RARED_VOLTAGE_250V) {
                    if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_normalIn_t->setText("6");
                    else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_normalIn_t->setText("6");
                    else if (mAttachmentType == NON_REWIRABLE_PLUG) {
                        if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "Tinsel") ui->lineEdit_normalIn_t->setText("1");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "0.5") ui->lineEdit_normalIn_t->setText("2.5");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "0.75") ui->lineEdit_normalIn_t->setText("6");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_normalIn_t->setText("6");
                        else  ui->lineEdit_normalIn_t->clear();
                    } else  ui->lineEdit_normalIn_t->clear();
                } else  ui->lineEdit_normalIn_t->clear();
            } else if (arg1 == RATED_CURRENT_10A) {
                if (ui->lineEdit_normalVn->text() >= RARED_VOLTAGE_130V &&
                        ui->lineEdit_normalVn->text() <= RARED_VOLTAGE_250V) {
                    if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_normalIn_t->setText("10");
                    else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_normalIn_t->setText("10");
                    else if (mAttachmentType == NON_REWIRABLE_PORTABLE_SOCKET_OUTLET) {
                        if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "0.75") ui->lineEdit_normalIn_t->setText("10");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_normalIn_t->setText("10");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_normalIn_t->setText("10");
                        else  ui->lineEdit_normalIn_t->clear();
                    } else if (mAttachmentType == NON_REWIRABLE_PLUG) {
                        if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "0.5") ui->lineEdit_normalIn_t->setText("2.5");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "0.75") ui->lineEdit_normalIn_t->setText("10");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_normalIn_t->setText("10");
                        else  ui->lineEdit_normalIn_t->clear();
                    } else  ui->lineEdit_normalIn_t->clear();
                } else if (ui->lineEdit_normalVn->text() == RARED_VOLTAGE_440V) {
                    if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_normalIn_t->setText("10");
                    else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_normalIn_t->setText("10");
                    else if (mAttachmentType == NON_REWIRABLE_PORTABLE_SOCKET_OUTLET) {
                        if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_normalIn_t->setText("10");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_normalIn_t->setText("10");
                        else  ui->lineEdit_normalIn_t->clear();
                    } else if (mAttachmentType == NON_REWIRABLE_PLUG) {
                        if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_normalIn_t->setText("10");
                        else  ui->lineEdit_normalIn_t->clear();
                    } else  ui->lineEdit_normalIn_t->clear();
                } else  ui->lineEdit_normalIn_t->clear();
            } else if (arg1 == RATED_CURRENT_13A) {
                if (ui->lineEdit_normalVn->text() == RARED_VOLTAGE_250V) {
                    if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_normalIn_t->setText("13");
                    else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_normalIn_t->setText("13");
                    else if (mAttachmentType == NON_REWIRABLE_PORTABLE_SOCKET_OUTLET) {
                        if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "0.75") ui->lineEdit_normalIn_t->setText("10");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_normalIn_t->setText("12");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_normalIn_t->setText("16");
                        else  ui->lineEdit_normalIn_t->clear();
                    } else if (mAttachmentType == NON_REWIRABLE_PLUG) {
                        if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "0.5") ui->lineEdit_normalIn_t->setText("2.5");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "0.75") ui->lineEdit_normalIn_t->setText("10");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_normalIn_t->setText("12");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_normalIn_t->setText("16");
                        else  ui->lineEdit_normalIn_t->clear();
                    } else  ui->lineEdit_normalIn_t->clear();
                } else  ui->lineEdit_normalIn_t->clear();
            } else if (arg1 == RATED_CURRENT_16A) {
                if (ui->lineEdit_normalVn->text() >= RARED_VOLTAGE_130V &&
                        ui->lineEdit_normalVn->text() <= RARED_VOLTAGE_250V) {
                    if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_normalIn_t->setText("16");
                    else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_normalIn_t->setText("16");
                    else if (mAttachmentType == NON_REWIRABLE_PORTABLE_SOCKET_OUTLET) {
                        if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_normalIn_t->setText("16");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_normalIn_t->setText("16");
                        else  ui->lineEdit_normalIn_t->clear();
                    }else if (mAttachmentType == NON_REWIRABLE_PLUG) {
                        if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "Tinsel") ui->lineEdit_normalIn_t->setText("1");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "0.5") ui->lineEdit_normalIn_t->setText("2.5");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "0.75") ui->lineEdit_normalIn_t->setText("10");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_normalIn_t->setText("12");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_normalIn_t->setText("16");
                        else  ui->lineEdit_normalIn_t->clear();
                    } else  ui->lineEdit_normalIn_t->clear();
                } else if (ui->lineEdit_normalVn->text() == RARED_VOLTAGE_440V) {
                    if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_normalIn_t->setText("16");
                    else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_normalIn_t->setText("16");
                    else if (mAttachmentType == NON_REWIRABLE_PORTABLE_SOCKET_OUTLET) {
                        if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_normalIn_t->setText("16");
                        else  ui->lineEdit_normalIn_t->clear();
                    } else if (mAttachmentType == NON_REWIRABLE_PLUG) {
                        if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_normalIn_t->setText("16");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "2.5") ui->lineEdit_normalIn_t->setText("22");
                        else  ui->lineEdit_normalIn_t->clear();
                    } else  ui->lineEdit_normalIn_t->clear();
                } else  ui->lineEdit_normalIn_t->clear();
            } else if (arg1 == RATED_CURRENT_25A) {
                if (ui->lineEdit_normalVn->text() == RARED_VOLTAGE_250V) {
                    if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_normalIn_t->setText("32");
                    else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_normalIn_t->setText("4");
                    else  ui->lineEdit_normalIn_t->clear();
                } else  ui->lineEdit_normalIn_t->clear();
            } else if (arg1 == RATED_CURRENT_32A) {
                if (ui->lineEdit_normalVn->text() >= RARED_VOLTAGE_130V &&
                        ui->lineEdit_normalVn->text() <= RARED_VOLTAGE_440V) {
                    if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_normalIn_t->setText("32");
                    else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_normalIn_t->setText("32");
                    else if (mAttachmentType == NON_REWIRABLE_PORTABLE_SOCKET_OUTLET) {
                        if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "2.5") ui->lineEdit_normalIn_t->setText("25");
                        else  ui->lineEdit_normalIn_t->clear();
                    } else if (mAttachmentType == NON_REWIRABLE_PLUG) {
                        if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "2.5") ui->lineEdit_normalIn_t->setText("25");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "4") ui->lineEdit_normalIn_t->setText("31");
                        else if (ui->tableWidget_4->item(mAttachmentType, 1)->text() == "6") ui->lineEdit_normalIn_t->setText("32");
                        else  ui->lineEdit_normalIn_t->clear();
                    } else  ui->lineEdit_normalIn_t->clear();
                } else  ui->lineEdit_normalIn_t->clear();
            } else ui->lineEdit_normalIn_t->clear();
        } else {
            ui->lineEdit_normalIn_t->clear();
            ui->lineEdit_normal_tongTime->clear();
        }
    }
}

void FormDisplay::on_lineEdit_normalVn_t_textChanged(const QString &arg1)
{
    if (ui->stackedWidget->currentIndex() == STACKWIDGET_884AUTO) {
        ui->lineEditVn_t_3->setText(arg1);
    }

    if (!arg1.isEmpty() && !ui->lineEdit_normalIn_t->text().isEmpty() &&
            !ui->lineEdit_normal_handleRate->text().isEmpty() &&
            !ui->lineEdit_normal_rate_t->text().isEmpty() &&
            !ui->lineEdit_normal_position_t->text().isEmpty()){
        ui->toolButton_save21->setEnabled(true);
        if (ui->stackedWidget->currentIndex() == 9 &&
                ui->toolButton_save19->isEnabled() &&
                ui->toolButton_save20->isEnabled() &&
                ui->toolButton_save22->isEnabled()) {
            ui->toolButtonSave->setEnabled(true);
        } else {
            ui->toolButtonSave->setEnabled(false);
        }
    } else {
        ui->toolButton_save21->setEnabled(false);
    }
}

void FormDisplay::on_lineEdit_normalIn_t_textChanged(const QString &arg1)
{
    if (ui->stackedWidget->currentIndex() == STACKWIDGET_884AUTO) {
        ui->lineEditIn_t_3->setText(arg1);
    }

    if (!arg1.isEmpty() && !ui->lineEdit_normal_tongTime->text().isEmpty() &&
            !ui->lineEdit_normal_rate_t->text().isEmpty() &&
            !ui->lineEdit_normal_position_t->text().isEmpty()){
        ui->toolButton_save21->setEnabled(true);
        if (ui->stackedWidget->currentIndex() == 9 &&
                ui->toolButton_save19->isEnabled() &&
                ui->toolButton_save20->isEnabled() &&
                ui->toolButton_save22->isEnabled()) {
            ui->toolButtonSave->setEnabled(true);
        } else {
            ui->toolButtonSave->setEnabled(false);
        }
    } else {
        ui->toolButton_save21->setEnabled(false);
    }
}

void FormDisplay::on_lineEdit_normal_rate_t_textChanged(const QString &arg1)
{
    if (!arg1.isEmpty() && !ui->lineEdit_normal_position_t->text().isEmpty() &&
            !ui->lineEdit_normalVn_t->text().isEmpty() &&
            !ui->lineEdit_normal_tongTime->text().isEmpty()){
        ui->toolButton_save21->setEnabled(true);
        if (ui->stackedWidget->currentIndex() == 9 &&
                ui->toolButton_save19->isEnabled() &&
                ui->toolButton_save20->isEnabled() &&
                ui->toolButton_save22->isEnabled()) {
            ui->toolButtonSave->setEnabled(true);
        } else {
            ui->toolButtonSave->setEnabled(false);
        }
    } else {
        ui->toolButton_save21->setEnabled(false);
    }
}

void FormDisplay::on_lineEdit_normal_position_t_textChanged(const QString &arg1)
{
    if (!arg1.isEmpty() && !ui->lineEdit_normal_rate_t->text().isEmpty() &&
            !ui->lineEdit_normalVn_t->text().isEmpty() &&
            !ui->lineEdit_normal_tongTime->text().isEmpty()){
        ui->toolButton_save21->setEnabled(true);
        if (ui->stackedWidget->currentIndex() == 9 &&
                ui->toolButton_save19->isEnabled() &&
                ui->toolButton_save20->isEnabled() &&
                ui->toolButton_save22->isEnabled()) {
            ui->toolButtonSave->setEnabled(true);
        } else {
            ui->toolButtonSave->setEnabled(false);
        }
    } else {
        ui->toolButton_save21->setEnabled(false);
    }
}

void FormDisplay::on_lineEdit_normalTVn_textChanged(const QString &arg1)
{
    if (!ui->checkBox_normalTemp->isChecked()) {
        ui->lineEdit_normalTVn_t->setText(arg1);
        on_lineEdit_normalTIn_textChanged(ui->lineEdit_normalTIn->text());
    }
}

void FormDisplay::on_lineEdit_normalTIn_textChanged(const QString &arg1)
{
    if (!ui->checkBox_normalTemp->isChecked()) {
        if (arg1 == RATED_CURRENT_2_5A) {
            if (ui->lineEdit_normalTVn->text() >= RARED_VOLTAGE_130V &&
                    ui->lineEdit_normalTVn->text() <= RARED_VOLTAGE_250V) {
                if (mAttachmentType == NON_REWIRABLE_PLUG) {
                    if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "Tinsel") ui->lineEdit_normalTIn_t->setText("1");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "0.5") ui->lineEdit_normalTIn_t->setText("2.5");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "0.75") ui->lineEdit_normalTIn_t->setText("2.5");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_normalTIn_t->setText("2.5");
                    else  ui->lineEdit_normalTIn_t->clear();
                } else  ui->lineEdit_normalTIn_t->clear();
            } else  ui->lineEdit_normalTIn_t->clear();
        } else if (arg1 == RATED_CURRENT_6A) {
            if (ui->lineEdit_normalTVn->text() >= RARED_VOLTAGE_130V &&
                    ui->lineEdit_normalTVn->text() <= RARED_VOLTAGE_250V) {
                if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_normalTIn_t->setText("6");
                else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_normalTIn_t->setText("6");
                else if (mAttachmentType == NON_REWIRABLE_PLUG) {
                    if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "Tinsel") ui->lineEdit_normalTIn_t->setText("1");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "0.5") ui->lineEdit_normalTIn_t->setText("2.5");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "0.75") ui->lineEdit_normalTIn_t->setText("6");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_normalTIn_t->setText("6");
                    else  ui->lineEdit_normalTIn_t->clear();
                } else  ui->lineEdit_normalTIn_t->clear();
            } else  ui->lineEdit_normalTIn_t->clear();
        } else if (arg1 == RATED_CURRENT_10A) {
            if (ui->lineEdit_normalTVn->text() >= RARED_VOLTAGE_130V &&
                    ui->lineEdit_normalTVn->text() <= RARED_VOLTAGE_250V) {
                if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_normalTIn_t->setText("10");
                else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_normalTIn_t->setText("10");
                else if (mAttachmentType == NON_REWIRABLE_PORTABLE_SOCKET_OUTLET) {
                    if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "0.75") ui->lineEdit_normalTIn_t->setText("10");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_normalTIn_t->setText("10");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_normalTIn_t->setText("10");
                    else  ui->lineEdit_normalTIn_t->clear();
                } else if (mAttachmentType == NON_REWIRABLE_PLUG) {
                    if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "0.5") ui->lineEdit_normalTIn_t->setText("2.5");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "0.75") ui->lineEdit_normalTIn_t->setText("10");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_normalTIn_t->setText("10");
                    else  ui->lineEdit_normalTIn_t->clear();
                } else  ui->lineEdit_normalTIn_t->clear();
            } else if (ui->lineEdit_normalTVn->text() == RARED_VOLTAGE_440V) {
                if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_normalTIn_t->setText("10");
                else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_normalTIn_t->setText("10");
                else if (mAttachmentType == NON_REWIRABLE_PORTABLE_SOCKET_OUTLET) {
                    if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_normalTIn_t->setText("10");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_normalTIn_t->setText("10");
                    else  ui->lineEdit_normalTIn_t->clear();
                } else if (mAttachmentType == NON_REWIRABLE_PLUG) {
                    if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_normalTIn_t->setText("10");
                    else  ui->lineEdit_normalTIn_t->clear();
                } else  ui->lineEdit_normalTIn_t->clear();
            } else  ui->lineEdit_normalTIn_t->clear();
        } else if (arg1 == RATED_CURRENT_13A) {
            if (ui->lineEdit_normalTVn->text() == RARED_VOLTAGE_250V) {
                if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_normalTIn_t->setText("13");
                else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_normalTIn_t->setText("13");
                else if (mAttachmentType == NON_REWIRABLE_PORTABLE_SOCKET_OUTLET) {
                    if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "0.75") ui->lineEdit_normalTIn_t->setText("10");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_normalTIn_t->setText("12");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_normalTIn_t->setText("16");
                    else  ui->lineEdit_normalTIn_t->clear();
                } else if (mAttachmentType == NON_REWIRABLE_PLUG) {
                    if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "0.5") ui->lineEdit_normalTIn_t->setText("2.5");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "0.75") ui->lineEdit_normalTIn_t->setText("10");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_normalTIn_t->setText("12");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_normalTIn_t->setText("16");
                    else  ui->lineEdit_normalTIn_t->clear();
                } else  ui->lineEdit_normalTIn_t->clear();
            } else  ui->lineEdit_normalTIn_t->clear();
        } else if (arg1 == RATED_CURRENT_16A) {
            if (ui->lineEdit_normalTVn->text() >= RARED_VOLTAGE_130V &&
                    ui->lineEdit_normalTVn->text() <= RARED_VOLTAGE_250V) {
                if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_normalTIn_t->setText("16");
                else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_normalTIn_t->setText("16");
                else if (mAttachmentType == NON_REWIRABLE_PORTABLE_SOCKET_OUTLET) {
                    if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_normalTIn_t->setText("16");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_normalTIn_t->setText("16");
                    else  ui->lineEdit_normalTIn_t->clear();
                }else if (mAttachmentType == NON_REWIRABLE_PLUG) {
                    if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "Tinsel") ui->lineEdit_normalTIn_t->setText("1");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "0.5") ui->lineEdit_normalTIn_t->setText("2.5");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "0.75") ui->lineEdit_normalTIn_t->setText("10");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "1") ui->lineEdit_normalTIn_t->setText("12");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_normalTIn_t->setText("16");
                    else  ui->lineEdit_normalTIn_t->clear();
                } else  ui->lineEdit_normalTIn_t->clear();
            } else if (ui->lineEdit_normalTVn->text() == RARED_VOLTAGE_440V) {
                if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_normalTIn_t->setText("16");
                else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_normalTIn_t->setText("16");
                else if (mAttachmentType == NON_REWIRABLE_PORTABLE_SOCKET_OUTLET) {
                    if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_normalTIn_t->setText("16");
                    else  ui->lineEdit_normalTIn_t->clear();
                } else if (mAttachmentType == NON_REWIRABLE_PLUG) {
                    if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "1.5") ui->lineEdit_normalTIn_t->setText("16");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "2.5") ui->lineEdit_normalTIn_t->setText("22");
                    else  ui->lineEdit_normalTIn_t->clear();
                } else  ui->lineEdit_normalTIn_t->clear();
            } else  ui->lineEdit_normalTIn_t->clear();
        } else if (arg1 == RATED_CURRENT_25A) {
            if (ui->lineEdit_normalTVn->text() == RARED_VOLTAGE_250V) {
                if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_normalTIn_t->setText("32");
                else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_normalTIn_t->setText("4");
                else  ui->lineEdit_normalTIn_t->clear();
            } else  ui->lineEdit_normalTIn_t->clear();
        } else if (arg1 == RATED_CURRENT_32A) {
            if (ui->lineEdit_normalTVn->text() >= RARED_VOLTAGE_130V &&
                    ui->lineEdit_normalTVn->text() <= RARED_VOLTAGE_440V) {
                if (mAttachmentType == REWIRABLE_FIXED_ACCESSORIES) ui->lineEdit_normalTIn_t->setText("32");
                else if (mAttachmentType == REWIRABLE_PORTABLE_ACCESSORIES) ui->lineEdit_normalTIn_t->setText("32");
                else if (mAttachmentType == NON_REWIRABLE_PORTABLE_SOCKET_OUTLET) {
                    if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "2.5") ui->lineEdit_normalTIn_t->setText("25");
                    else  ui->lineEdit_normalTIn_t->clear();
                } else if (mAttachmentType == NON_REWIRABLE_PLUG) {
                    if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "2.5") ui->lineEdit_normalTIn_t->setText("25");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "4") ui->lineEdit_normalTIn_t->setText("31");
                    else if (ui->tableWidget_5->item(mAttachmentType, 1)->text() == "6") ui->lineEdit_normalTIn_t->setText("32");
                    else  ui->lineEdit_normalTIn_t->clear();
                } else  ui->lineEdit_normalTIn_t->clear();
            } else  ui->lineEdit_normalTIn_t->clear();
        } else ui->lineEdit_normalTIn_t->clear();

        if (!ui->lineEdit_normalTIn_t->text().isEmpty() &&
                !ui->lineEdit_normalTVn_t->text().isEmpty()) {
            ui->toolButton_save21->setEnabled(true);
            if (ui->stackedWidget->currentIndex() == 9 &&
                    ui->toolButton_save19->isEnabled() &&
                    ui->toolButton_save20->isEnabled() &&
                    ui->toolButton_save22->isEnabled()) {
                ui->toolButtonSave->setEnabled(true);
            } else {
                ui->toolButtonSave->setEnabled(false);
            }
        }
    }
}

void FormDisplay::on_lineEdit_normalTIn_t_textChanged(const QString &arg1)
{
    if (ui->stackedWidget->currentIndex() == STACKWIDGET_884AUTO) {
        ui->lineEditIn_t_4->setText(arg1);
    }

    if (!arg1.isEmpty() &&
            !ui->lineEdit_normalTVn_t->text().isEmpty() &&
            !ui->lineEdit_normalTH->text().isEmpty()){
        ui->toolButton_save22->setEnabled(true);
        if (ui->stackedWidget->currentIndex() == 9 &&
                ui->toolButton_save19->isEnabled() &&
                ui->toolButton_save20->isEnabled() &&
                ui->toolButton_save22->isEnabled()) {
            ui->toolButtonSave->setEnabled(true);
        } else {
            ui->toolButtonSave->setEnabled(false);
        }
    } else {
        ui->toolButton_save22->setEnabled(false);
    }
}

void FormDisplay::on_lineEdit_normalTH_textChanged(const QString &arg1)
{
    if (!arg1.isEmpty() &&
            !ui->lineEdit_normalTVn_t->text().isEmpty() &&
            !ui->lineEdit_normalTIn_t->text().isEmpty()){
        ui->toolButton_save22->setEnabled(true);
        if (ui->stackedWidget->currentIndex() == 9 &&
                ui->toolButton_save19->isEnabled() &&
                ui->toolButton_save20->isEnabled() &&
                ui->toolButton_save22->isEnabled()) {
            ui->toolButtonSave->setEnabled(true);
        } else {
            ui->toolButtonSave->setEnabled(false);
        }
    } else {
        ui->toolButton_save22->setEnabled(false);
    }
}

void FormDisplay::on_tableWidget_temp_cellChanged(int row, int column)
{
    if (column == 0) {
        if (row >= 0 && row < 20) {
            QTableWidgetItem* item = ui->tableWidget_temp->item(row,column);
            if(!item) return;

            //0-999
            QString str = item->text();
            QRegExp rxs("^[0-9]{1,3}$");
            if (!rxs.exactMatch(str)) {
                item->setText(tr(""));
            }
            if (item->text().toInt() > 400) {
                item->setText("400");
            }
        }
    }
}

void FormDisplay::on_tableWidget_normalTemp_cellChanged(int row, int column)
{
    if (column == 0) {
        if (row >= 0 && row < 20) {
            QTableWidgetItem* item = ui->tableWidget_normalTemp->item(row,column);
            if(!item) return;

            //0-999
            QString str = item->text();
            QRegExp rxs("^[0-9]{1,3}$");
            if (!rxs.exactMatch(str)) {
                item->setText(tr(""));
            }
            if (item->text().toInt() > 400) {
                item->setText("400");
            }
        }
    }
}

void FormDisplay::on_tableWidget_temp_2_cellChanged(int row, int column)
{
    if (column == 0) {
        if (row >= 0 && row < 20) {
            QTableWidgetItem* item = ui->tableWidget_temp->item(row,column);
            if(!item) return;
            //0-999
            QString str = item->text();
            QRegExp rxs("^[0-9]{1,3}$");
            if (!rxs.exactMatch(str)) {
                item->setText(tr(""));
            }
            if (item->text().toInt() > 400) {
                item->setText("400");
            }
        }
    }
}

void FormDisplay::on_lineEdit_tempVn_2_textChanged(const QString &arg1)
{
    if (!ui->checkBox_temp_2->isChecked()) {
        if (!arg1.isEmpty()){
            ui->lineEdit_tempVn_t_2->setText(arg1);
        } else {
            ui->lineEdit_tempVn_t_2->clear();
        }
    }
}

void FormDisplay::on_lineEdit_tempIn_2_textChanged(const QString &arg1)
{
    if (!ui->checkBox_temp_2->isChecked()) {
        if (!arg1.isEmpty()){
            ui->lineEdit_tempIn_t_2->setText(QString::number(arg1.toDouble()*1.25));
        } else {
            ui->lineEdit_tempIn_t_2->clear();
        }
    }
}

void FormDisplay::on_lineEdit_tempH_2_textChanged(const QString &arg1)
{
    if (!arg1.isEmpty() && !ui->lineEdit_tempIn_t_2->text().isEmpty() &&
            !ui->lineEdit_tempVn_t_2->text().isEmpty()){
        ui->toolButton_save19_2->setEnabled(true);
        if (ui->stackedWidget->currentIndex() == 10 &&
                ui->toolButton_save20_2->isEnabled() &&
                ui->toolButton_save21_2->isEnabled()) {
            ui->toolButtonSave_2->setEnabled(true);
        } else {
            ui->toolButtonSave_2->setEnabled(false);
        }
    } else {
        ui->toolButton_save19_2->setEnabled(false);
    }
}

void FormDisplay::on_lineEdit_tempVn_t_2_textChanged(const QString &arg1)
{
    if (ui->stackedWidget->currentIndex() == STACKWIDGET_320AUTO) {
        ui->lineEditVn_t_5->setText(arg1);
    }

    if (!arg1.isEmpty() && !ui->lineEdit_tempIn_t_2->text().isEmpty() &&
            !ui->lineEdit_tempH_2->text().isEmpty()){
        ui->toolButton_save19_2->setEnabled(true);
        if (ui->stackedWidget->currentIndex() == 10 &&
                ui->toolButton_save20_2->isEnabled() &&
                ui->toolButton_save21_2->isEnabled()) {
            ui->toolButtonSave_2->setEnabled(true);
        } else {
            ui->toolButtonSave_2->setEnabled(false);
        }
    } else {
        ui->toolButton_save19_2->setEnabled(false);
    }
}

void FormDisplay::on_lineEdit_tempIn_t_2_textChanged(const QString &arg1)
{
    if (ui->stackedWidget->currentIndex() == STACKWIDGET_320AUTO) {
        ui->lineEditIn_t_5->setText(arg1);
    }

    if (!arg1.isEmpty() && !ui->lineEdit_tempVn_t_2->text().isEmpty() &&
            !ui->lineEdit_tempH_2->text().isEmpty()){
        ui->toolButton_save19_2->setEnabled(true);
        if (ui->stackedWidget->currentIndex() == 10 &&
                ui->toolButton_save20_2->isEnabled() &&
                ui->toolButton_save21_2->isEnabled()) {
            ui->toolButtonSave_2->setEnabled(true);
        } else {
            ui->toolButtonSave_2->setEnabled(false);
        }
    } else {
        ui->toolButton_save19_2->setEnabled(false);
    }
}

void FormDisplay::on_toolButton_save19_2_clicked()
{
    if (!this->isChecked(ui->tableWidget_temp_2->item(ui->lineEdit_tempMaxValue2_2->text().toInt()-1, 0))) {
        ui->tableWidget_temp_2->item(ui->lineEdit_tempMaxValue2_2->text().toInt()-1, 0)->setCheckState(Qt::Checked);
    }

    if (ui->tableWidget_temp_2->item(ui->lineEdit_tempMaxValue2_2->text().toInt()-1, 0)->text().toInt() != 25) {
        ui->tableWidget_temp_2->item(ui->lineEdit_tempMaxValue2_2->text().toInt()-1, 0)->setText("25");
    }

    addItemIcon(IMAGE_CLAUSE19, CLAUSE_19);
}

void FormDisplay::on_lineEdit_breakingVn_2_textChanged(const QString &arg1)
{
    if (!ui->checkBox_breaking_2->isChecked()) {
        if (!arg1.isEmpty()){
            ui->lineEdit_breakingVn_t_2->setText(QString::number(arg1.toDouble() * 1.1));
        } else {
            ui->lineEdit_breakingVn_t_2->clear();
        }
    }
}

void FormDisplay::on_lineEdit_breakingIn_2_textChanged(const QString &arg1)
{
    if (!arg1.isEmpty()){
        if (arg1.toDouble() > 0.2)
            if (!ui->checkBox_breaking_2->isChecked()) {
                ui->lineEdit_breakingIn_t_2->setText(QString::number(arg1.toDouble() * 1.25));
            }

        if (arg1.toDouble() < STACKWIDGET_320AUTO) {
            ui->lineEdit_breakingRn_2->setText(tr("Inductive Load(0.6)"));
        } else {
            ui->lineEdit_breakingRn_2->setText(tr("Resistive Load"));
        }
    } else {
        ui->lineEdit_breakingIn_t_2->clear();
    }
}

void FormDisplay::on_lineEdit_breaking_rate_t_2_textChanged(const QString &arg1)
{
    if (!arg1.isEmpty() && !ui->lineEdit_breaking_position_t_2->text().isEmpty() &&
            !ui->lineEdit_breakingIn_t_2->text().isEmpty() &&
            !ui->lineEdit_breakingVn_t_2->text().isEmpty()){
        ui->toolButton_save20_2->setEnabled(true);
        if (ui->stackedWidget->currentIndex() == 10 &&
                ui->toolButton_save19_2->isEnabled() &&
                ui->toolButton_save21_2->isEnabled()) {
            ui->toolButtonSave_2->setEnabled(true);
        } else {
            ui->toolButtonSave_2->setEnabled(false);
        }
    } else {
        ui->toolButton_save20_2->setEnabled(false);
    }
}

void FormDisplay::on_lineEdit_breaking_position_t_2_textChanged(const QString &arg1)
{
    if (!arg1.isEmpty() && !ui->lineEdit_breaking_rate_t_2->text().isEmpty() &&
            !ui->lineEdit_breakingIn_t_2->text().isEmpty() &&
            !ui->lineEdit_breakingVn_t_2->text().isEmpty()){
        ui->toolButton_save20_2->setEnabled(true);
        if (ui->stackedWidget->currentIndex() == 10 &&
                ui->toolButton_save19_2->isEnabled() &&
                ui->toolButton_save21_2->isEnabled()) {
            ui->toolButtonSave_2->setEnabled(true);
        } else {
            ui->toolButtonSave_2->setEnabled(false);
        }
    } else {
        ui->toolButton_save20_2->setEnabled(false);
    }
}

void FormDisplay::on_lineEdit_breakingVn_t_2_textChanged(const QString &arg1)
{
    if (ui->checkBox_breaking_2->isChecked()) {
        ui->lineEdit_breakingVn_2->setText(arg1);
    }

    if (ui->stackedWidget->currentIndex() == STACKWIDGET_320AUTO) {
        ui->lineEditVn_t_6->setText(arg1);
    }

    if (!arg1.isEmpty() && !ui->lineEdit_breakingIn_t_2->text().isEmpty() &&
            !ui->lineEdit_breaking_rate_t_2->text().isEmpty() &&
            !ui->lineEdit_breaking_position_t_2->text().isEmpty()){
        ui->toolButton_save20_2->setEnabled(true);
        if (ui->stackedWidget->currentIndex() == 10 &&
                ui->toolButton_save19_2->isEnabled() &&
                ui->toolButton_save21_2->isEnabled()) {
            ui->toolButtonSave_2->setEnabled(true);
        } else {
            ui->toolButtonSave_2->setEnabled(false);
        }
    } else {
        ui->toolButton_save20_2->setEnabled(false);
    }
}

void FormDisplay::on_lineEdit_breakingIn_t_2_textChanged(const QString &arg1)
{
    if (ui->checkBox_breaking_2->isChecked()) {
        ui->lineEdit_breakingIn_2->setText(arg1);
    }

    if (ui->stackedWidget->currentIndex() == STACKWIDGET_320AUTO) {
        ui->lineEditIn_t_6->setText(arg1);
    }

    if (!arg1.isEmpty() && !ui->lineEdit_breaking_rate_t_2->text().isEmpty() &&
            !ui->lineEdit_breaking_position_t_2->text().isEmpty()){
        ui->toolButton_save20_2->setEnabled(true);
        if (ui->stackedWidget->currentIndex() == 10 &&
                ui->toolButton_save19_2->isEnabled() &&
                ui->toolButton_save21_2->isEnabled()) {
            ui->toolButtonSave_2->setEnabled(true);
        } else {
            ui->toolButtonSave_2->setEnabled(false);
        }
    } else {
        ui->toolButton_save20_2->setEnabled(false);
    }
}

void FormDisplay::on_toolButton_save20_2_clicked()
{
    addItemIcon(IMAGE_CLAUSE20, CLAUSE_20);
}

void FormDisplay::on_lineEdit_normalVn_2_textChanged(const QString &arg1)
{
    if (!ui->checkBox_normal_2->isChecked()) {
        if (!ui->lineEdit_normalIn_2->text().isEmpty()) {
            if (0 < ui->lineEdit_normalIn_2->text().toDouble() && ui->lineEdit_normalIn_2->text().toDouble() <= 0.2) {
                ui->lineEdit_normalVn_t_2->setText("0");
            } else if (0.2 < ui->lineEdit_normalIn_2->text().toDouble() && ui->lineEdit_normalIn_2->text().toDouble() < 10) {
                ui->lineEdit_normalVn_t_2->setText(arg1);
            } else if (10 <= ui->lineEdit_normalIn_2->text().toDouble()) {
                ui->lineEdit_normalVn_t_2->setText(arg1);
            } else {
                ui->lineEdit_normalVn_t_2->clear();
            }
        }
    }
}

void FormDisplay::on_lineEdit_normalIn_2_textChanged(const QString &arg1)
{
    if (0 < arg1.toDouble() && arg1.toDouble() <= 0.2) {
        if (!ui->checkBox_normal_2->isChecked()) {
            ui->lineEdit_normalVn_t_2->setText("0");
            ui->lineEdit_normalIn_t_2->setText("0");
            ui->lineEdit_normal_handleNum_2->setText(tr("4000"));
            ui->lineEdit_normalRn_2->setText("-");
        }
    } else if (0.2 < arg1.toDouble() && arg1.toDouble() < 10) {
        if (!ui->checkBox_normal->isChecked()) {
            ui->lineEdit_normalIn_t_2->setText(ui->lineEdit_normalIn_2->text());
            ui->lineEdit_normal_handleNum_2->setText("8000");
            if (!ui->lineEdit_normalVn_2->text().isEmpty()){
                ui->lineEdit_normalVn_t_2->setText(ui->lineEdit_normalVn_2->text());
            }
        }
        ui->lineEdit_normalRn_2->setText(tr("Inductive Load(0.6)"));
    } else if (10 <= arg1.toDouble()) {
        if (!ui->checkBox_normal->isChecked()) {
            ui->lineEdit_normalIn_t_2->setText(ui->lineEdit_normalIn_2->text());
            ui->lineEdit_normal_handleNum_2->setText("8000");
            if (!ui->lineEdit_normalVn_2->text().isEmpty()){
                ui->lineEdit_normalVn_t_2->setText(ui->lineEdit_normalVn_2->text());
            }
        }
        ui->lineEdit_normalRn_2->setText(tr("Resistive Load"));
    } else {
        ui->lineEdit_normalVn_t_2->clear();
        ui->lineEdit_normalIn_t_2->clear();
        ui->lineEdit_normal_handleNum_2->clear();
    }
}

void FormDisplay::on_lineEdit_normal_rate_t_2_textChanged(const QString &arg1)
{
    if (!arg1.isEmpty() && !ui->lineEdit_normal_position_t_2->text().isEmpty() &&
            !ui->lineEdit_normalVn_t_2->text().isEmpty() &&
            !ui->lineEdit_normalIn_t_2->text().isEmpty() &&
            !ui->lineEdit_normal_handleNum_2->text().isEmpty()){
        ui->toolButton_save21_2->setEnabled(true);
        if (ui->stackedWidget->currentIndex() == 10 &&
                ui->toolButton_save20_2->isEnabled() &&
                ui->toolButton_save19_2->isEnabled()) {
            ui->toolButtonSave_2->setEnabled(true);
        } else {
            ui->toolButtonSave_2->setEnabled(false);
        }
    } else {
        ui->toolButton_save21_2->setEnabled(false);
    }
}

void FormDisplay::on_lineEdit_normal_position_t_2_textChanged(const QString &arg1)
{
    if (!arg1.isEmpty() && !ui->lineEdit_normal_rate_t_2->text().isEmpty() &&
            !ui->lineEdit_normalVn_t_2->text().isEmpty() &&
            !ui->lineEdit_normalIn_t_2->text().isEmpty() &&
            !ui->lineEdit_normal_handleNum_2->text().isEmpty()){
        ui->toolButton_save21_2->setEnabled(true);
        if (ui->stackedWidget->currentIndex() == 10 &&
                ui->toolButton_save20_2->isEnabled() &&
                ui->toolButton_save19_2->isEnabled()) {
            ui->toolButtonSave_2->setEnabled(true);
        } else {
            ui->toolButtonSave_2->setEnabled(false);
        }
    } else {
        ui->toolButton_save21_2->setEnabled(false);
    }
}

void FormDisplay::on_lineEdit_normalVn_t_2_textChanged(const QString &arg1)
{
    if (ui->stackedWidget->currentIndex() == STACKWIDGET_320AUTO) {
        ui->lineEditVn_t_7->setText(arg1);
    }

    if (!arg1.isEmpty() && !ui->lineEdit_normal_position_t_2->text().isEmpty() &&
            !ui->lineEdit_normal_rate_t_2->text().isEmpty() &&
            !ui->lineEdit_normalIn_t_2->text().isEmpty() &&
            !ui->lineEdit_normal_handleNum_2->text().isEmpty()){
        ui->toolButton_save21_2->setEnabled(true);
        if (ui->stackedWidget->currentIndex() == 10 &&
                ui->toolButton_save20_2->isEnabled() &&
                ui->toolButton_save19_2->isEnabled()) {
            ui->toolButtonSave_2->setEnabled(true);
        } else {
            ui->toolButtonSave_2->setEnabled(false);
        }
    } else {
        ui->toolButton_save21_2->setEnabled(false);
    }
}

void FormDisplay::on_lineEdit_normalIn_t_2_textChanged(const QString &arg1)
{
    if (ui->stackedWidget->currentIndex() == STACKWIDGET_320AUTO) {
        ui->lineEditIn_t_7->setText(arg1);
    }

    if (!arg1.isEmpty() && !ui->lineEdit_normal_position_t_2->text().isEmpty() &&
            !ui->lineEdit_normalVn_t_2->text().isEmpty() &&
            !ui->lineEdit_normal_rate_t_2->text().isEmpty() &&
            !ui->lineEdit_normal_handleNum_2->text().isEmpty()){
        ui->toolButton_save21_2->setEnabled(true);
        if (ui->stackedWidget->currentIndex() == 10 &&
                ui->toolButton_save20_2->isEnabled() &&
                ui->toolButton_save19_2->isEnabled()) {
            ui->toolButtonSave_2->setEnabled(true);
        } else {
            ui->toolButtonSave_2->setEnabled(false);
        }
    } else {
        ui->toolButton_save21_2->setEnabled(false);
    }
}

void FormDisplay::on_lineEdit_normal_handleNum_2_textChanged(const QString &arg1)
{
    if (ui->stackedWidget->currentIndex() == STACKWIDGET_320AUTO) {
        ui->lineEditHandleNum_5->setText(arg1);
    }

    if (!arg1.isEmpty() && !ui->lineEdit_normal_position_t_2->text().isEmpty() &&
            !ui->lineEdit_normalVn_t_2->text().isEmpty() &&
            !ui->lineEdit_normalIn_t_2->text().isEmpty() &&
            !ui->lineEdit_normal_rate_t_2->text().isEmpty()){
        ui->toolButton_save21_2->setEnabled(true);
        if (ui->stackedWidget->currentIndex() == 10 &&
                ui->toolButton_save20_2->isEnabled() &&
                ui->toolButton_save19_2->isEnabled()) {
            ui->toolButtonSave_2->setEnabled(true);
        } else {
            ui->toolButtonSave_2->setEnabled(false);
        }
    } else {
        ui->toolButton_save21_2->setEnabled(false);
    }
}

void FormDisplay::on_toolButton_save21_2_clicked()
{
    addItemIcon(IMAGE_CLAUSE21, CLAUSE_21);
}

void FormDisplay::on_tabWidget_currentChanged(int index)
{
    switch (index) {
    case TABWIDGET_SERVOA:
    {
        mServo = servoA;
        break;
    }
    case TABWIDGET_SERVOB:
    {
        mServo = servoB;
        break;
    }
    case TABWIDGET_SERVOC:
    {
        mServo = servoC;
        break;
    }
    }
}

void FormDisplay::on_toolButton_start_2_clicked()
{
    if (!connectSTAS) {
        QMessageBox msgBox(QMessageBox::Warning,tr("Error"),tr("Connect STAS False!\nPlease after the restart to try again."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
        msgBox.setWindowIcon(QIcon(WINDOW_ICON));

        msgBox.exec();
        return;
    }

    if (!mPiecesPowerListB->count()) {
        mMessage->show();
        mMessage->setText(tr("Please Add Item!"));
        return;
    }

    if (!ui->checkBox_VOLT->isChecked()) {
        QMessageBox msgBox(QMessageBox::Warning,tr("Error"),tr("Power Supply UART was disabled!\nPlease check and update the UART Set."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
        msgBox.setWindowIcon(QIcon(WINDOW_ICON));

        msgBox.exec();
        return;
    }

    if (!ui->checkBox_B->isChecked()) {
        QMessageBox msgBox(QMessageBox::Warning,tr("Error"),tr("Load(#5) UART was disabled!\nPlease check and update the UART Set."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
        msgBox.setWindowIcon(QIcon(WINDOW_ICON));

        msgBox.exec();
        return;
    }

    dirNameListB.clear();

    if (!mPiecesPowerListB->count()) {
        mMessage->show();
        mMessage->setText(tr("Please Add Item!"));
        return;
    }

    for (int i = 0; i < mPiecesPowerListB->count(); i++)
    {
        QString fileName = mPiecesPowerListB->item(i)->text();
        if (fileName.right(5).mid(0,2).toInt() == 19 || fileName.right(5).mid(0,2).toInt() == 22) {
            if (!ui->checkBox_TRC->isChecked()) {
                QMessageBox msgBox(QMessageBox::Warning,tr("Error"),tr("Paperless Recorder UART was disabled!\nPlease check and update the UART Set."));
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
                msgBox.setWindowIcon(QIcon(WINDOW_ICON));

                msgBox.exec();
                return;
            }
        } else if (fileName.right(5).mid(0,2).toInt() == 20 || fileName.right(5).mid(0,2).toInt() == 21) {
            if (!ui->checkBox_TEST->isChecked()) {
                QMessageBox msgBox(QMessageBox::Warning,tr("Error"),tr("Life Tester UART was disabled!\nPlease check and update the UART Set."));
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
                msgBox.setWindowIcon(QIcon(WINDOW_ICON));

                msgBox.exec();
                return;
            }
        }
        dirNameListB << dirName+fileName;
    }

    ui->toolButton_start_2->setEnabled(false);
    mPiecesPowerListB->setAcceptDrops(false);
    mPiecesPowerListB->setDragEnabled(false);
    emit isSectionFileName(dirNameListB.at(0), mPlug);
}

void FormDisplay::on_toolButton_start_3_clicked()
{
    if (!connectSTAS) {
        QMessageBox msgBox(QMessageBox::Warning,tr("Error"),tr("Connect STAS False!\nPlease after the restart to try again."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
        msgBox.setWindowIcon(QIcon(WINDOW_ICON));

        msgBox.exec();
        return;
    }

    if (!mPiecesPowerListC->count()) {
        mMessage->show();
        mMessage->setText(tr("Please Add Item!"));
        return;
    }

    if (!ui->checkBox_VOLT->isChecked()) {
        QMessageBox msgBox(QMessageBox::Warning,tr("Error"),tr("Power Supply UART was disabled!\nPlease check and update the UART Set."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
        msgBox.setWindowIcon(QIcon(WINDOW_ICON));

        msgBox.exec();
        return;
    }

    if (!ui->checkBox_C->isChecked()) {
        QMessageBox msgBox(QMessageBox::Warning,tr("Error"),tr("Load(#3) UART was disabled!\nPlease check and update the UART Set."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
        msgBox.setWindowIcon(QIcon(WINDOW_ICON));

        msgBox.exec();
        return;
    }

    dirNameListC.clear();

    if (!mPiecesPowerListC->count()) {
        mMessage->show();
        mMessage->setText(tr("Please Add Item!"));
        return;
    }

    for (int i = 0; i < mPiecesPowerListC->count(); i++)
    {
        QString fileName = mPiecesPowerListC->item(i)->text();
        if (fileName.right(5).mid(0,2).toInt() == 19 || fileName.right(5).mid(0,2).toInt() == 22) {
            if (!ui->checkBox_TRC->isChecked()) {
                QMessageBox msgBox(QMessageBox::Warning,tr("Error"),tr("Paperless Recorder UART was disabled!\nPlease check and update the UART Set."));
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
                msgBox.setWindowIcon(QIcon(WINDOW_ICON));

                msgBox.exec();
                return;
            }
        } else if (fileName.right(5).mid(0,2).toInt() == 20 || fileName.right(5).mid(0,2).toInt() == 21) {
            if (!ui->checkBox_TEST->isChecked()) {
                QMessageBox msgBox(QMessageBox::Warning,tr("Error"),tr("Life Tester UART was disabled!\nPlease check and update the UART Set."));
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
                msgBox.setWindowIcon(QIcon(WINDOW_ICON));

                msgBox.exec();
                return;
            }
        }
        dirNameListC << dirName+fileName;
    }

    ui->toolButton_start_3->setEnabled(false);
    mPiecesPowerListC->setAcceptDrops(false);
    mPiecesPowerListC->setDragEnabled(false);
    emit isSectionFileName(dirNameListC.at(0), mPlug);
}

void FormDisplay::on_lineEdit_VOLTUART_textChanged(const QString &arg1)
{
    if (arg1.left(3) == "COM" && arg1.size() == 4) {
        if (arg1.right(1).toInt()>=0 && arg1.right(1).toInt()<=7) {
            if (arg1.right(1) == ui->lineEdit_TESTUART->text().right(1)) {
                ui->lineEdit_TESTUART->setText(QString("COM%1").arg(comVolt));
                comTest = comVolt;
            } else if (arg1.right(1) == ui->lineEdit_TRCUART->text().right(1)) {
                ui->lineEdit_TRCUART->setText(QString("COM%1").arg(comVolt));
                comTRC = comVolt;
            } else if (arg1.right(1) == ui->lineEdit_LOADUART_A->text().right(1)) {
                ui->lineEdit_LOADUART_A->setText(QString("COM%1").arg(comVolt));
                comLoadA = comVolt;
            } else if (arg1.right(1) == ui->lineEdit_LOADUART_B->text().right(1)) {
                ui->lineEdit_LOADUART_B->setText(QString("COM%1").arg(comVolt));
                comLoadB = comVolt;
            } else if (arg1.right(1) == ui->lineEdit_LOADUART_C->text().right(1)) {
                ui->lineEdit_LOADUART_C->setText(QString("COM%1").arg(comVolt));
                comLoadC = comVolt;
            }
            comVolt = arg1.right(1).toInt();
        }
    }
    ui->pushButton_4->setEnabled(true);
}

void FormDisplay::on_lineEdit_TESTUART_textChanged(const QString &arg1)
{
    if (arg1.left(3) == "COM" && arg1.size() == 4) {
        if (arg1.right(1).toInt()>=0 && arg1.right(1).toInt()<=7) {
            if (arg1.right(1) == ui->lineEdit_VOLTUART->text().right(1)) {
                ui->lineEdit_VOLTUART->setText(QString("COM%1").arg(comTest));
                comVolt = comTest;
            } else if (arg1.right(1) == ui->lineEdit_TRCUART->text().right(1)) {
                ui->lineEdit_TRCUART->setText(QString("COM%1").arg(comTest));
                comTRC = comTest;
            } else if (arg1.right(1) == ui->lineEdit_LOADUART_A->text().right(1)) {
                ui->lineEdit_LOADUART_A->setText(QString("COM%1").arg(comTest));
                comLoadA = comTest;
            } else if (arg1.right(1) == ui->lineEdit_LOADUART_B->text().right(1)) {
                ui->lineEdit_LOADUART_B->setText(QString("COM%1").arg(comTest));
                comLoadB = comTest;
            } else if (arg1.right(1) == ui->lineEdit_LOADUART_C->text().right(1)) {
                ui->lineEdit_LOADUART_C->setText(QString("COM%1").arg(comTest));
                comLoadC = comTest;
            }
            comTest = arg1.right(1).toInt();
        }
    }
    ui->pushButton_4->setEnabled(true);
}

void FormDisplay::on_lineEdit_TRCUART_textChanged(const QString &arg1)
{
    if (arg1.left(3) == "COM" && arg1.size() == 4) {
        if (arg1.right(1).toInt()>=0 && arg1.right(1).toInt()<=7) {
            if (arg1.right(1) == ui->lineEdit_TESTUART->text().right(1)) {
                ui->lineEdit_TESTUART->setText(QString("COM%1").arg(comTRC));
                comTest = comTRC;
            } else if (arg1.right(1) == ui->lineEdit_VOLTUART->text().right(1)) {
                ui->lineEdit_VOLTUART->setText(QString("COM%1").arg(comTRC));
                comVolt = comTRC;
            } else if (arg1.right(1) == ui->lineEdit_LOADUART_A->text().right(1)) {
                ui->lineEdit_LOADUART_A->setText(QString("COM%1").arg(comTRC));
                comLoadA = comTRC;
            } else if (arg1.right(1) == ui->lineEdit_LOADUART_B->text().right(1)) {
                ui->lineEdit_LOADUART_B->setText(QString("COM%1").arg(comTRC));
                comLoadB = comTRC;
            } else if (arg1.right(1) == ui->lineEdit_LOADUART_C->text().right(1)) {
                ui->lineEdit_LOADUART_C->setText(QString("COM%1").arg(comTRC));
                comLoadC = comTRC;
            }
            comTRC = arg1.right(1).toInt();
        }
    }
    ui->pushButton_4->setEnabled(true);
}

void FormDisplay::on_lineEdit_LOADUART_A_textChanged(const QString &arg1)
{
    if (arg1.left(3) == "COM" && arg1.size() == 4) {
        if (arg1.right(1).toInt()>=0 && arg1.right(1).toInt()<=7) {
            if (arg1.right(1) == ui->lineEdit_TESTUART->text().right(1)) {
                ui->lineEdit_TESTUART->setText(QString("COM%1").arg(comLoadA));
                comTest = comLoadA;
            } else if (arg1.right(1) == ui->lineEdit_TRCUART->text().right(1)) {
                ui->lineEdit_TRCUART->setText(QString("COM%1").arg(comLoadA));
                comTRC = comLoadA;
            } else if (arg1.right(1) == ui->lineEdit_VOLTUART->text().right(1)) {
                ui->lineEdit_VOLTUART->setText(QString("COM%1").arg(comLoadA));
                comVolt = comLoadA;
            } else if (arg1.right(1) == ui->lineEdit_LOADUART_B->text().right(1)) {
                ui->lineEdit_LOADUART_B->setText(QString("COM%1").arg(comLoadA));
                comLoadB = comLoadA;
            } else if (arg1.right(1) == ui->lineEdit_LOADUART_C->text().right(1)) {
                ui->lineEdit_LOADUART_C->setText(QString("COM%1").arg(comLoadA));
                comLoadC = comLoadA;
            }
            comLoadA = arg1.right(1).toInt();
        }
    }
    ui->pushButton_4->setEnabled(true);
}

void FormDisplay::on_lineEdit_LOADUART_B_textChanged(const QString &arg1)
{
    if (arg1.left(3) == "COM" && arg1.size() == 4) {
        if (arg1.right(1).toInt()>=0 && arg1.right(1).toInt()<=7) {
            if (arg1.right(1) == ui->lineEdit_TESTUART->text().right(1)) {
                ui->lineEdit_TESTUART->setText(QString("COM%1").arg(comLoadB));
                comTest = comLoadB;
            } else if (arg1.right(1) == ui->lineEdit_TRCUART->text().right(1)) {
                ui->lineEdit_TRCUART->setText(QString("COM%1").arg(comLoadB));
                comTRC = comLoadB;
            } else if (arg1.right(1) == ui->lineEdit_LOADUART_A->text().right(1)) {
                ui->lineEdit_LOADUART_A->setText(QString("COM%1").arg(comLoadB));
                comLoadA = comLoadB;
            } else if (arg1.right(1) == ui->lineEdit_VOLTUART->text().right(1)) {
                ui->lineEdit_VOLTUART->setText(QString("COM%1").arg(comLoadB));
                comVolt = comLoadB;
            } else if (arg1.right(1) == ui->lineEdit_LOADUART_C->text().right(1)) {
                ui->lineEdit_LOADUART_C->setText(QString("COM%1").arg(comLoadB));
                comLoadC = comLoadB;
            }
            comLoadB = arg1.right(1).toInt();
        }
    }
    ui->pushButton_4->setEnabled(true);
}

void FormDisplay::on_lineEdit_LOADUART_C_textChanged(const QString &arg1)
{
    if (arg1.left(3) == "COM" && arg1.size() == 4) {
        if (arg1.right(1).toInt()>=0 && arg1.right(1).toInt()<=7) {
            if (arg1.right(1) == ui->lineEdit_TESTUART->text().right(1)) {
                ui->lineEdit_TESTUART->setText(QString("COM%1").arg(comLoadC));
                comTest = comLoadC;
            } else if (arg1.right(1) == ui->lineEdit_TRCUART->text().right(1)) {
                ui->lineEdit_TRCUART->setText(QString("COM%1").arg(comLoadC));
                comTRC = comLoadC;
            } else if (arg1.right(1) == ui->lineEdit_LOADUART_A->text().right(1)) {
                ui->lineEdit_LOADUART_A->setText(QString("COM%1").arg(comLoadC));
                comLoadA = comLoadC;
            } else if (arg1.right(1) == ui->lineEdit_LOADUART_B->text().right(1)) {
                ui->lineEdit_LOADUART_B->setText(QString("COM%1").arg(comLoadC));
                comLoadB = comLoadC;
            } else if (arg1.right(1) == ui->lineEdit_VOLTUART->text().right(1)) {
                ui->lineEdit_VOLTUART->setText(QString("COM%1").arg(comLoadC));
                comVolt = comLoadC;
            }
            comLoadC = arg1.right(1).toInt();
        }
    }
    ui->pushButton_4->setEnabled(true);
}

void FormDisplay::on_pushButton_clicked()
{
    QMessageBox msgBox(QMessageBox::Question,tr(" Electrical Accessories Test Automation Program"),tr("Whether to start Auto?"));
    msgBox.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
    msgBox.setButtonText(QMessageBox::Cancel,tr("Cancel"));
    msgBox.setWindowIcon(QIcon(WINDOW_ICON));

    int ret = msgBox.exec();

    if (ret == QMessageBox::Cancel) {
        return;
    }

    switch (mPlug) {
    case IEC60320:
    {
        ui->tableWidget_2->hide();
        ui->toolButtonSave_2->setEnabled(false);

        IEC320_clear();
        ui->stackedWidget->setCurrentIndex(STACKWIDGET_320AUTO);
        break;
    }
    case IEC60884:
    {
        ui->labelIm->hide();
        ui->lineEditIm->hide();
        ui->tableWidget->hide();
        ui->toolButtonSave->setEnabled(false);

        IEC884_clear();
        ui->stackedWidget->setCurrentIndex(STACKWIDGET_884AUTO);
        break;
    }
    }
}

void FormDisplay::on_radioButtonSwitch_toggled(bool checked)
{
    ui->checkBox_tempSwitch->setChecked(checked);
    ui->checkBox_normalTSwitch->setChecked(checked);
}

void FormDisplay::on_radioButtonIC_toggled(bool checked)
{
    ui->checkBox_tempIC->setChecked(checked);

    if (checked) {
        ui->labelIm->show();
        ui->lineEditIm->show();
    } else {
        ui->labelIm->hide();
        ui->lineEditIm->hide();
        ui->lineEditIm->clear();
    }
}

void FormDisplay::on_lineEditIm_textChanged(const QString &arg1)
{
    ui->lineEdit_tempIm->setText(arg1);
}

void FormDisplay::on_lineEditVn_textChanged(const QString &arg1)
{
    ui->lineEdit_tempVn->setText(arg1);
    ui->lineEdit_breakingVn->setText(arg1);
    ui->lineEdit_normalVn->setText(arg1);
    ui->lineEdit_normalTVn->setText(arg1);
}

void FormDisplay::on_lineEditIn_textChanged(const QString &arg1)
{
    ui->lineEdit_tempIn->setText(arg1);
    ui->lineEdit_breakingIn->setText(arg1);
    ui->lineEdit_normalIn->setText(arg1);
    ui->lineEdit_normalTIn->setText(arg1);
}

void FormDisplay::on_lineEditH_textChanged(const QString &arg1)
{
    ui->lineEdit_tempH->setText(arg1);
    ui->lineEdit_normalTH->setText(arg1);
}

void FormDisplay::on_lineEditRate_textChanged(const QString &arg1)
{
    ui->lineEdit_breaking_rate_t->setText(arg1);
    ui->lineEdit_normal_rate_t->setText(arg1);
}

void FormDisplay::on_lineEditPosition_textChanged(const QString &arg1)
{
    ui->lineEdit_breaking_position_t->setText(arg1);
    ui->lineEdit_normal_position_t->setText(arg1);
}

void FormDisplay::on_toolButton_TRCR_clicked()
{
    if (ui->tableWidget->isHidden()){
        ui->tableWidget->show();
        ui->textEdit_5->hide();
    } else{
        ui->tableWidget->hide();
        ui->textEdit_5->show();
    }
}

void FormDisplay::on_lineEditMaxValue_textChanged(const QString &arg1)
{
    ui->lineEdit_tempMaxValue2->setText(arg1);
    ui->lineEdit_normalMaxValue2->setText(arg1);
}

void FormDisplay::on_lineEdit_tempVn_t_textChanged(const QString &arg1)
{
    if (ui->stackedWidget->currentIndex() == STACKWIDGET_884AUTO) {
        ui->lineEditVn_t_1->setText(arg1);
    }
}

void FormDisplay::on_lineEdit_breaking_handleRate_textChanged(const QString &arg1)
{
    if (ui->stackedWidget->currentIndex() == STACKWIDGET_884AUTO) {
        ui->lineEditHandleRate_1->setText(arg1);
    }
}

void FormDisplay::on_lineEdit_breaking_tongTime_textChanged(const QString &arg1)
{
    if (ui->stackedWidget->currentIndex() == STACKWIDGET_884AUTO) {
        ui->lineEditTongTime_1->setText(arg1);
    }
}

void FormDisplay::on_lineEdit_normal_handleRate_textChanged(const QString &arg1)
{
    if (ui->stackedWidget->currentIndex() == STACKWIDGET_884AUTO) {
        ui->lineEditHandleRate_2->setText(arg1);
    }
}

void FormDisplay::on_lineEdit_normal_tongTime_textChanged(const QString &arg1)
{
    if (ui->stackedWidget->currentIndex() == STACKWIDGET_884AUTO) {
        ui->lineEditTongTime_2->setText(arg1);
    }
}

void FormDisplay::on_lineEdit_normalTVn_t_textChanged(const QString &arg1)
{
    if (ui->stackedWidget->currentIndex() == STACKWIDGET_884AUTO) {
        ui->lineEditVn_t_4->setText(arg1);
    }
}

void FormDisplay::on_toolButtonSave_clicked()
{
    if (mServo == servoA) {
        while (mPiecesPowerListA->count())
            itemRestore(mPiecesPowerListA->item(0));
    } else if (mServo == servoB) {
        while (mPiecesPowerListB->count())
            itemRestore(mPiecesPowerListB->item(0));
    } else if (mServo == servoC) {
        while (mPiecesPowerListC->count())
            itemRestore(mPiecesPowerListC->item(0));
    }

    ui->tableWidget->setCurrentItem(ui->tableWidget->item(1,0));
    isAuto = true;
    on_toolButton_save19_clicked();
    isAuto = true;
    on_toolButton_save20_clicked();
    isAuto = true;
    on_toolButton_save21_clicked();
    isAuto = true;
    on_toolButton_save22_clicked();

    if (mServo == servoA) {
        moveToZone(mPiecesViewListA->item(mFileNameListA.indexOf("clause19_A0")));
        moveToZone(mPiecesViewListA->item(mFileNameListA.indexOf("clause20_A0")-1));
        moveToZone(mPiecesViewListA->item(mFileNameListA.indexOf("clause21_A0")-2));
        moveToZone(mPiecesViewListA->item(mFileNameListA.indexOf("clause22_A0")-3));

//        on_toolButton_start_clicked();
    } else if (mServo == servoB) {
        moveToZone(mPiecesViewListB->item(mFileNameListB.indexOf("clause19_B0")));
        moveToZone(mPiecesViewListB->item(mFileNameListB.indexOf("clause20_B0")-1));
        moveToZone(mPiecesViewListB->item(mFileNameListB.indexOf("clause21_B0")-2));
        moveToZone(mPiecesViewListB->item(mFileNameListB.indexOf("clause22_B0")-3));

//        on_toolButton_start_2_clicked();
    } else if (mServo == servoC) {
        moveToZone(mPiecesViewListC->item(mFileNameListC.indexOf("clause19_C0")));
        moveToZone(mPiecesViewListC->item(mFileNameListC.indexOf("clause20_C0")-1));
        moveToZone(mPiecesViewListC->item(mFileNameListC.indexOf("clause21_C0")-2));
        moveToZone(mPiecesViewListC->item(mFileNameListC.indexOf("clause22_C0")-3));

//        on_toolButton_start_3_clicked();
    }
}

void FormDisplay::on_tableWidget_itemChanged(QTableWidgetItem *item)
{
    if (isChecked(item)) {
        ui->tableWidget_temp->item(item->row(), item->column())->setCheckState(Qt::Checked);
        ui->tableWidget_normalTemp->item(item->row(), item->column())->setCheckState(Qt::Checked);
    } else {
        ui->tableWidget_temp->item(item->row(), item->column())->setCheckState(Qt::Unchecked);
        ui->tableWidget_normalTemp->item(item->row(), item->column())->setCheckState(Qt::Unchecked);
    }

    ui->tableWidget_temp->item(item->row(), item->column())->setText(item->text());
    ui->tableWidget_normalTemp->item(item->row(), item->column())->setText(item->text());
}

void FormDisplay::on_lineEditVn_2_textChanged(const QString &arg1)
{
    ui->lineEdit_tempVn_2->setText(arg1);
    ui->lineEdit_breakingVn_2->setText(arg1);
    ui->lineEdit_normalVn_2->setText(arg1);
}

void FormDisplay::on_lineEditIn_2_textChanged(const QString &arg1)
{
    ui->lineEdit_tempIn_2->setText(arg1);
    ui->lineEdit_breakingIn_2->setText(arg1);
    ui->lineEdit_normalIn_2->setText(arg1);
}

void FormDisplay::on_lineEditH_2_textChanged(const QString &arg1)
{
    ui->lineEdit_tempH_2->setText(arg1);
}

void FormDisplay::on_lineEditRate_2_textChanged(const QString &arg1)
{
    ui->lineEdit_breaking_rate_t_2->setText(arg1);
    ui->lineEdit_normal_rate_t_2->setText(arg1);
}

void FormDisplay::on_lineEditPosition_2_textChanged(const QString &arg1)
{
    ui->lineEdit_breaking_position_t_2->setText(arg1);
    ui->lineEdit_normal_position_t_2->setText(arg1);
}

void FormDisplay::on_toolButton_TRCR_2_clicked()
{
    if (ui->tableWidget_2->isHidden()){
        ui->tableWidget_2->show();
    } else {
        ui->tableWidget_2->hide();
    }
}

void FormDisplay::on_lineEditMaxValue_2_textChanged(const QString &arg1)
{
    ui->lineEdit_tempMaxValue2_2->setText(arg1);
}

void FormDisplay::on_tableWidget_2_itemChanged(QTableWidgetItem *item)
{
    if (isChecked(item)) {
        ui->tableWidget_temp_2->item(item->row(), item->column())->setCheckState(Qt::Checked);
    } else {
        ui->tableWidget_temp_2->item(item->row(), item->column())->setCheckState(Qt::Unchecked);
    }

    ui->tableWidget_temp_2->item(item->row(), item->column())->setText(item->text());
}

void FormDisplay::on_lineEdit_breakingRn_2_textChanged(const QString &arg1)
{
    ui->lineEditRn_6->setText(arg1);
}

void FormDisplay::on_lineEdit_normalRn_2_textChanged(const QString &arg1)
{
    ui->lineEditRn_7->setText(arg1);
}

void FormDisplay::on_toolButtonSave_2_clicked()
{
    if (mServo == servoA) {
        while (mPiecesPowerListA->count())
            itemRestore(mPiecesPowerListA->item(0));
    } else if (mServo == servoB) {
        while (mPiecesPowerListB->count())
            itemRestore(mPiecesPowerListB->item(0));
    } else if (mServo == servoC) {
        while (mPiecesPowerListC->count())
            itemRestore(mPiecesPowerListC->item(0));
    }

    ui->tableWidget_2->setCurrentItem(ui->tableWidget_2->item(1,0));
    isAuto = true;
    on_toolButton_save19_2_clicked();
    isAuto = true;
    on_toolButton_save20_2_clicked();
    isAuto = true;
    on_toolButton_save21_2_clicked();

    if (mServo == servoA) {
        moveToZone(mPiecesViewListA->item(mFileNameListA.indexOf("clause19_A0")));
        moveToZone(mPiecesViewListA->item(mFileNameListA.indexOf("clause20_A0")-1));
        moveToZone(mPiecesViewListA->item(mFileNameListA.indexOf("clause21_A0")-2));

//        on_toolButton_start_clicked();
    } else if (mServo == servoB) {
        moveToZone(mPiecesViewListB->item(mFileNameListB.indexOf("clause19_B0")));
        moveToZone(mPiecesViewListB->item(mFileNameListB.indexOf("clause20_B0")-1));
        moveToZone(mPiecesViewListB->item(mFileNameListB.indexOf("clause21_B0")-2));

//        on_toolButton_start_2_clicked();
    } else if (mServo == servoC) {
        moveToZone(mPiecesViewListC->item(mFileNameListC.indexOf("clause19_C0")));
        moveToZone(mPiecesViewListC->item(mFileNameListC.indexOf("clause20_C0")-1));
        moveToZone(mPiecesViewListC->item(mFileNameListC.indexOf("clause21_C0")-2));

//        on_toolButton_start_3_clicked();
    }
}

void FormDisplay::on_tableWidget_3_itemChanged(QTableWidgetItem *item)
{
    if (item->column() == 0) {
        switch (item->row()) {
        case 0:
        {
            if (!isChecked(ui->tableWidget_3->item(1,0)) && !isChecked(ui->tableWidget_3->item(2,0)) && !isChecked(ui->tableWidget_3->item(3,0))) {
                if (!isManual && !isChecked(item)) {
                    item->setCheckState(Qt::Checked);
                }
            }

            if (!isManual && isChecked(ui->tableWidget_3->item(1,0))) {
                isManual = true;
                ui->tableWidget_3->item(1,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_3->item(2,0))) {
                isManual = true;
                ui->tableWidget_3->item(2,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_3->item(3,0))) {
                isManual = true;
                ui->tableWidget_3->item(3,0)->setCheckState(Qt::Unchecked);
            }

            if (!isManual) {
                ui->tableWidget_3->item(2,1)->setText("");
                ui->tableWidget_3->item(3,1)->setText("");
            }

            mAttachmentType = 0;
            break;
        }
        case 1:
        {
            if (!isChecked(ui->tableWidget_3->item(0,0)) && !isChecked(ui->tableWidget_3->item(2,0)) && !isChecked(ui->tableWidget_3->item(3,0))) {
                if (!isManual && !isChecked(item)) {
                    item->setCheckState(Qt::Checked);
                }
            }

            if (!isManual && isChecked(ui->tableWidget_3->item(0,0))) {
                isManual = true;
                ui->tableWidget_3->item(0,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_3->item(2,0))) {
                isManual = true;
                ui->tableWidget_3->item(2,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_3->item(3,0))) {
                isManual = true;
                ui->tableWidget_3->item(3,0)->setCheckState(Qt::Unchecked);
            }

            if (!isManual) {
                ui->tableWidget_3->item(2,1)->setText("");
                ui->tableWidget_3->item(3,1)->setText("");
            }

            mAttachmentType = 1;
            break;
        }
        case 2:
        {
            if (!isChecked(ui->tableWidget_3->item(1,0)) && !isChecked(ui->tableWidget_3->item(0,0)) && !isChecked(ui->tableWidget_3->item(3,0))) {
                if (!isManual && !isChecked(item)) {
                    item->setCheckState(Qt::Checked);
                }
            }

            if (!isManual && isChecked(ui->tableWidget_3->item(0,0))) {
                isManual = true;
                ui->tableWidget_3->item(0,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_3->item(1,0))) {
                isManual = true;
                ui->tableWidget_3->item(1,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_3->item(3,0))) {
                isManual = true;
                ui->tableWidget_3->item(3,0)->setCheckState(Qt::Unchecked);
            }

            if (!isManual) {
                ui->tableWidget_3->item(3,1)->setText("");
            }

            mAttachmentType = 2;
            break;
        }
        case 3:
        {
            if (!isChecked(ui->tableWidget_3->item(1,0)) && !isChecked(ui->tableWidget_3->item(2,0)) && !isChecked(ui->tableWidget_3->item(0,0))) {
                if (!isManual && !isChecked(item)) {
                    item->setCheckState(Qt::Checked);
                }
            }

            if (!isManual && isChecked(ui->tableWidget_3->item(0,0))) {
                isManual = true;
                ui->tableWidget_3->item(0,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_3->item(2,0))) {
                isManual = true;
                ui->tableWidget_3->item(2,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_3->item(3,0))) {
                isManual = true;
                ui->tableWidget_3->item(1,0)->setCheckState(Qt::Unchecked);
            }

            if (!isManual) {
                ui->tableWidget_3->item(2,1)->setText("");
            }

            mAttachmentType = 3;
            break;
        }
        }
    }

    isManual = false;
    on_lineEdit_tempIn_textChanged(ui->lineEdit_tempIn->text());
}

void FormDisplay::on_tableWidget_4_itemChanged(QTableWidgetItem *item)
{
    if (item->column() == 0) {
        switch (item->row()) {
        case 0:
        {
            if (!isChecked(ui->tableWidget_4->item(1,0)) && !isChecked(ui->tableWidget_4->item(2,0)) && !isChecked(ui->tableWidget_4->item(3,0))) {
                if (!isManual && !isChecked(item)) {
                    item->setCheckState(Qt::Checked);
                }
            }

            if (!isManual && isChecked(ui->tableWidget_4->item(1,0))) {
                isManual = true;
                ui->tableWidget_4->item(1,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_4->item(2,0))) {
                isManual = true;
                ui->tableWidget_4->item(2,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_4->item(3,0))) {
                isManual = true;
                ui->tableWidget_4->item(3,0)->setCheckState(Qt::Unchecked);
            }

            if (!isManual) {
                ui->tableWidget_4->item(2,1)->setText("");
                ui->tableWidget_4->item(3,1)->setText("");
            }

            mAttachmentType = 0;
            break;
        }
        case 1:
        {
            if (!isChecked(ui->tableWidget_4->item(0,0)) && !isChecked(ui->tableWidget_4->item(2,0)) && !isChecked(ui->tableWidget_4->item(3,0))) {
                if (!isManual && !isChecked(item)) {
                    item->setCheckState(Qt::Checked);
                }
            }

            if (!isManual && isChecked(ui->tableWidget_4->item(0,0))) {
                isManual = true;
                ui->tableWidget_4->item(0,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_4->item(2,0))) {
                isManual = true;
                ui->tableWidget_4->item(2,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_4->item(3,0))) {
                isManual = true;
                ui->tableWidget_4->item(3,0)->setCheckState(Qt::Unchecked);
            }

            if (!isManual) {
                ui->tableWidget_4->item(2,1)->setText("");
                ui->tableWidget_4->item(3,1)->setText("");
            }

            mAttachmentType = 1;
            break;
        }
        case 2:
        {
            if (!isChecked(ui->tableWidget_4->item(1,0)) && !isChecked(ui->tableWidget_4->item(0,0)) && !isChecked(ui->tableWidget_4->item(3,0))) {
                if (!isManual && !isChecked(item)) {
                    item->setCheckState(Qt::Checked);
                }
            }

            if (!isManual && isChecked(ui->tableWidget_4->item(0,0))) {
                isManual = true;
                ui->tableWidget_4->item(0,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_4->item(1,0))) {
                isManual = true;
                ui->tableWidget_4->item(1,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_4->item(3,0))) {
                isManual = true;
                ui->tableWidget_4->item(3,0)->setCheckState(Qt::Unchecked);
            }

            if (!isManual) {
                ui->tableWidget_4->item(3,1)->setText("");
            }

            mAttachmentType = 2;
            break;
        }
        case 3:
        {
            if (!isChecked(ui->tableWidget_4->item(1,0)) && !isChecked(ui->tableWidget_4->item(2,0)) && !isChecked(ui->tableWidget_4->item(0,0))) {
                if (!isManual && !isChecked(item)) {
                    item->setCheckState(Qt::Checked);
                }
            }

            if (!isManual && isChecked(ui->tableWidget_4->item(0,0))) {
                isManual = true;
                ui->tableWidget_4->item(0,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_4->item(2,0))) {
                isManual = true;
                ui->tableWidget_4->item(2,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_4->item(3,0))) {
                isManual = true;
                ui->tableWidget_4->item(1,0)->setCheckState(Qt::Unchecked);
            }

            if (!isManual) {
                ui->tableWidget_4->item(2,1)->setText("");
            }

            mAttachmentType = 3;
            break;
        }
        }
    }

    isManual = false;
    on_lineEdit_normalIn_textChanged(ui->lineEdit_normalIn->text());
}

void FormDisplay::on_tableWidget_5_itemChanged(QTableWidgetItem *item)
{
    if (item->column() == 0) {
        switch (item->row()) {
        case 0:
        {
            if (!isChecked(ui->tableWidget_5->item(1,0)) && !isChecked(ui->tableWidget_5->item(2,0)) && !isChecked(ui->tableWidget_5->item(3,0))) {
                if (!isManual && !isChecked(item)) {
                    item->setCheckState(Qt::Checked);
                }
            }

            if (!isManual && isChecked(ui->tableWidget_5->item(1,0))) {
                isManual = true;
                ui->tableWidget_5->item(1,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_5->item(2,0))) {
                isManual = true;
                ui->tableWidget_5->item(2,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_5->item(3,0))) {
                isManual = true;
                ui->tableWidget_5->item(3,0)->setCheckState(Qt::Unchecked);
            }

            if (!isManual) {
                ui->tableWidget_5->item(2,1)->setText("");
                ui->tableWidget_5->item(3,1)->setText("");
            }

            mAttachmentType = 0;
            break;
        }
        case 1:
        {
            if (!isChecked(ui->tableWidget_5->item(0,0)) && !isChecked(ui->tableWidget_5->item(2,0)) && !isChecked(ui->tableWidget_5->item(3,0))) {
                if (!isManual && !isChecked(item)) {
                    item->setCheckState(Qt::Checked);
                }
            }

            if (!isManual && isChecked(ui->tableWidget_5->item(0,0))) {
                isManual = true;
                ui->tableWidget_5->item(0,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_5->item(2,0))) {
                isManual = true;
                ui->tableWidget_5->item(2,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_5->item(3,0))) {
                isManual = true;
                ui->tableWidget_5->item(3,0)->setCheckState(Qt::Unchecked);
            }

            if (!isManual) {
                ui->tableWidget_5->item(2,1)->setText("");
                ui->tableWidget_5->item(3,1)->setText("");
            }

            mAttachmentType = 1;
            break;
        }
        case 2:
        {
            if (!isChecked(ui->tableWidget_5->item(1,0)) && !isChecked(ui->tableWidget_5->item(0,0)) && !isChecked(ui->tableWidget_5->item(3,0))) {
                if (!isManual && !isChecked(item)) {
                    item->setCheckState(Qt::Checked);
                }
            }

            if (!isManual && isChecked(ui->tableWidget_5->item(0,0))) {
                isManual = true;
                ui->tableWidget_5->item(0,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_5->item(1,0))) {
                isManual = true;
                ui->tableWidget_5->item(1,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_5->item(3,0))) {
                isManual = true;
                ui->tableWidget_5->item(3,0)->setCheckState(Qt::Unchecked);
            }

            if (!isManual) {
                ui->tableWidget_5->item(3,1)->setText("");
            }

            mAttachmentType = 2;
            break;
        }
        case 3:
        {
            if (!isChecked(ui->tableWidget_5->item(1,0)) && !isChecked(ui->tableWidget_5->item(2,0)) && !isChecked(ui->tableWidget_5->item(0,0))) {
                if (!isManual && !isChecked(item)) {
                    item->setCheckState(Qt::Checked);
                }
            }

            if (!isManual && isChecked(ui->tableWidget_5->item(0,0))) {
                isManual = true;
                ui->tableWidget_5->item(0,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_5->item(2,0))) {
                isManual = true;
                ui->tableWidget_5->item(2,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_5->item(3,0))) {
                isManual = true;
                ui->tableWidget_5->item(1,0)->setCheckState(Qt::Unchecked);
            }

            if (!isManual) {
                ui->tableWidget_5->item(2,1)->setText("");
            }

            mAttachmentType = 3;
            break;
        }
        }
    }

    isManual = false;
    on_lineEdit_normalTIn_textChanged(ui->lineEdit_normalTIn->text());
}

void FormDisplay::on_tableWidget_6_itemChanged(QTableWidgetItem *item)
{
    if (item->column() == 0) {
        switch (item->row()) {
        case 0:
        {
            if (!isChecked(ui->tableWidget_6->item(1,0)) && !isChecked(ui->tableWidget_6->item(2,0)) && !isChecked(ui->tableWidget_6->item(3,0))) {
                if (!isManual && !isChecked(item)) {
                    item->setCheckState(Qt::Checked);
                }
            }

            if (!isManual && isChecked(ui->tableWidget_6->item(1,0))) {
                isManual = true;
                ui->tableWidget_6->item(1,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_6->item(2,0))) {
                isManual = true;
                ui->tableWidget_6->item(2,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_6->item(3,0))) {
                isManual = true;
                ui->tableWidget_6->item(3,0)->setCheckState(Qt::Unchecked);
            }

            if (!isManual) {
                ui->tableWidget_6->item(2,1)->setText("");
                ui->tableWidget_6->item(3,1)->setText("");
            }

            mAttachmentType = 0;
            break;
        }
        case 1:
        {
            if (!isChecked(ui->tableWidget_6->item(0,0)) && !isChecked(ui->tableWidget_6->item(2,0)) && !isChecked(ui->tableWidget_6->item(3,0))) {
                if (!isManual && !isChecked(item)) {
                    item->setCheckState(Qt::Checked);
                }
            }

            if (!isManual && isChecked(ui->tableWidget_6->item(0,0))) {
                isManual = true;
                ui->tableWidget_6->item(0,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_6->item(2,0))) {
                isManual = true;
                ui->tableWidget_6->item(2,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_6->item(3,0))) {
                isManual = true;
                ui->tableWidget_6->item(3,0)->setCheckState(Qt::Unchecked);
            }

            if (!isManual) {
                ui->tableWidget_6->item(2,1)->setText("");
                ui->tableWidget_6->item(3,1)->setText("");
            }

            mAttachmentType = 1;
            break;
        }
        case 2:
        {
            if (!isChecked(ui->tableWidget_6->item(1,0)) && !isChecked(ui->tableWidget_6->item(0,0)) && !isChecked(ui->tableWidget_6->item(3,0))) {
                if (!isManual && !isChecked(item)) {
                    item->setCheckState(Qt::Checked);
                }
            }

            if (!isManual && isChecked(ui->tableWidget_6->item(0,0))) {
                isManual = true;
                ui->tableWidget_6->item(0,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_6->item(1,0))) {
                isManual = true;
                ui->tableWidget_6->item(1,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_6->item(3,0))) {
                isManual = true;
                ui->tableWidget_6->item(3,0)->setCheckState(Qt::Unchecked);
            }

            if (!isManual) {
                ui->tableWidget_6->item(3,1)->setText("");
            }

            mAttachmentType = 2;
            break;
        }
        case 3:
        {
            if (!isChecked(ui->tableWidget_6->item(1,0)) && !isChecked(ui->tableWidget_6->item(2,0)) && !isChecked(ui->tableWidget_6->item(0,0))) {
                if (!isManual && !isChecked(item)) {
                    item->setCheckState(Qt::Checked);
                }
            }

            if (!isManual && isChecked(ui->tableWidget_6->item(0,0))) {
                isManual = true;
                ui->tableWidget_6->item(0,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_6->item(2,0))) {
                isManual = true;
                ui->tableWidget_6->item(2,0)->setCheckState(Qt::Unchecked);
            } else if (!isManual && isChecked(ui->tableWidget_6->item(3,0))) {
                isManual = true;
                ui->tableWidget_6->item(1,0)->setCheckState(Qt::Unchecked);
            }

            if (!isManual) {
                ui->tableWidget_6->item(2,1)->setText("");
            }

            mAttachmentType = 3;
            break;
        }
        }
    }

    isManual = false;

    ui->tableWidget_3->item(mAttachmentType,0)->setCheckState(Qt::Checked);
    ui->tableWidget_4->item(mAttachmentType,0)->setCheckState(Qt::Checked);
    ui->tableWidget_5->item(mAttachmentType,0)->setCheckState(Qt::Checked);
    if (mAttachmentType == 2 || mAttachmentType == 3) {
        ui->tableWidget_3->item(mAttachmentType,1)->setText(item->text());
        ui->tableWidget_4->item(mAttachmentType,1)->setText(item->text());
        ui->tableWidget_5->item(mAttachmentType,1)->setText(item->text());
    }
}

void FormDisplay::on_checkBox_B_toggled(bool checked)
{
    Q_UNUSED(checked);

    if (devInformation.at(3).data.isEmpty() || devInformation.at(3).data.size() < 16){
        ui->checkBox_B->setChecked(false);
    } else {
        ui->checkBox_B->setChecked(true);
    }
}

void FormDisplay::on_checkBox_VOLT_toggled(bool checked)
{
    Q_UNUSED(checked);

    if (devInformation.at(0).data.isEmpty() || devInformation.at(0).data.size() != 14){
        ui->checkBox_VOLT->setChecked(false);
    } else {
        ui->checkBox_VOLT->setChecked(true);
    }
}

void FormDisplay::on_checkBox_TRC_toggled(bool checked)
{
    Q_UNUSED(checked);

    if (devInformation.at(1).data.isEmpty() || devInformation.at(1).data.size() < 27){
        ui->checkBox_TRC->setChecked(false);
    } else {
        ui->checkBox_TRC->setChecked(true);
    }
}

void FormDisplay::on_checkBox_TEST_toggled(bool checked)
{
    Q_UNUSED(checked);

    if (devInformation.at(5).data.isEmpty() || devInformation.at(5).data.size() != 13){
        ui->checkBox_TEST->setChecked(false);
    } else {
        ui->checkBox_TEST->setChecked(true);
    }
}

void FormDisplay::on_checkBox_C_toggled(bool checked)
{
    Q_UNUSED(checked);

    if (devInformation.at(4).data.isEmpty() || devInformation.at(4).data.size() < 16){
        ui->checkBox_C->setChecked(false);
    } else {
        ui->checkBox_C->setChecked(true);
    }
}

void FormDisplay::on_checkBox_A_toggled(bool checked)
{
    Q_UNUSED(checked);

    if (devInformation.at(2).data.isEmpty() || devInformation.at(2).data.size() < 16){
        ui->checkBox_A->setChecked(false);
    } else {
        ui->checkBox_A->setChecked(true);
    }
}

void FormDisplay::on_pushButton_4_clicked()
{
    if (ui->lineEdit_VOLTUART->text().left(3) != "COM" ||
            ui->lineEdit_VOLTUART->text().right(1).toInt()<0 ||
            ui->lineEdit_VOLTUART->text().right(1).toInt()>7) {
        QMessageBox msgBox(QMessageBox::Warning,tr(" Electrical Accessories Test Automation Program"),
                           tr("Please enter the correct Power Supply parameters!\nE.g, COM0"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
        msgBox.setWindowIcon(QIcon(WINDOW_ICON));

        msgBox.exec();
        ui->pushButton_4->setEnabled(false);
        return;
    } else if (ui->lineEdit_TRCUART->text().left(3) != "COM" ||
            ui->lineEdit_TRCUART->text().right(1).toInt()<0 ||
            ui->lineEdit_TRCUART->text().right(1).toInt()>7) {
        QMessageBox msgBox(QMessageBox::Warning,tr(" Electrical Accessories Test Automation Program"),
                           tr("Please enter the correct Paperless Recorder parameters!\nE.g, COM0"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
        msgBox.setWindowIcon(QIcon(WINDOW_ICON));

        msgBox.exec();
        ui->pushButton_4->setEnabled(false);
        return;
    } else if (ui->lineEdit_TESTUART->text().left(3) != "COM" ||
            ui->lineEdit_TESTUART->text().right(1).toInt()<0 ||
            ui->lineEdit_TESTUART->text().right(1).toInt()>7) {
        QMessageBox msgBox(QMessageBox::Warning,tr(" Electrical Accessories Test Automation Program"),
                           tr("Please enter the correct life tester parameters!\nE.g, COM0"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
        msgBox.setWindowIcon(QIcon(WINDOW_ICON));

        msgBox.exec();
        ui->pushButton_4->setEnabled(false);
        return;
    } else if (ui->lineEdit_LOADUART_A->text().left(3) != "COM" ||
            ui->lineEdit_LOADUART_A->text().right(1).toInt()<0 ||
            ui->lineEdit_LOADUART_A->text().right(1).toInt()>7) {
        QMessageBox msgBox(QMessageBox::Warning,tr(" Electrical Accessories Test Automation Program"),
                           tr("Please enter the correct Load(servoA) parameters!\nE.g, COM0"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
        msgBox.setWindowIcon(QIcon(WINDOW_ICON));

        msgBox.exec();
        ui->pushButton_4->setEnabled(false);
        return;
    } else if (ui->lineEdit_LOADUART_B->text().left(3) != "COM" ||
            ui->lineEdit_LOADUART_B->text().right(1).toInt()<0 ||
            ui->lineEdit_LOADUART_B->text().right(1).toInt()>7) {
        QMessageBox msgBox(QMessageBox::Warning,tr(" Electrical Accessories Test Automation Program"),
                           tr("Please enter the correct Load(servoB) parameters!\nE.g, COM0"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
        msgBox.setWindowIcon(QIcon(WINDOW_ICON));

        msgBox.exec();
        ui->pushButton_4->setEnabled(false);
        return;
    } else if (ui->lineEdit_LOADUART_C->text().left(3) != "COM" ||
            ui->lineEdit_LOADUART_C->text().right(1).toInt()<0 ||
            ui->lineEdit_LOADUART_C->text().right(1).toInt()>7) {
        QMessageBox msgBox(QMessageBox::Warning,tr(" Electrical Accessories Test Automation Program"),
                           tr("Please enter the correct Load(servoC) parameters!\nE.g, COM0"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
        msgBox.setWindowIcon(QIcon(WINDOW_ICON));

        msgBox.exec();
        ui->pushButton_4->setEnabled(false);
        return;
    }

    QString volt = CollectControl::HardSend(ui->lineEdit_VOLTUART->text().right(1).toInt(), QString("RDW VF").toLatin1().data(), 1000);
    devInformation.removeAt(0);
    UART_t voltUart;
    voltUart.data = volt;
    comVolt = ui->lineEdit_VOLTUART->text().right(1).toInt();
    voltUart.com = comVolt;
    devInformation.insert(0,voltUart);
    if (volt.isEmpty() || volt.size() != 14) {
        ui->checkBox_VOLT->setChecked(false);
    } else {
        ui->checkBox_VOLT->setChecked(true);
    }

    QString tc = CollectControl::HardSend(ui->lineEdit_TRCUART->text().right(1).toInt(), QString("FData,0,0001,0110").toLatin1().data(), 1000);
    devInformation.removeAt(1);
    UART_t trUart;
    trUart.data = tc;
    comTRC = ui->lineEdit_TRCUART->text().right(1).toInt();
    trUart.com = comTRC;
    devInformation.insert(1,trUart);
    if (tc.isEmpty() || tc.size() != 723) {
        ui->checkBox_TRC->setChecked(false);
    } else {
        ui->checkBox_TRC->setChecked(true);
    }

    QString loadA = CollectControl::HardSend(ui->lineEdit_LOADUART_A->text().right(1).toInt(), QString("RDM%101%102%\r").toLatin1().data(), 1000);
    devInformation.removeAt(2);
    UART_t loadAUart;
    loadAUart.data = loadA;
    comLoadA = ui->lineEdit_LOADUART_A->text().right(1).toInt();
    loadAUart.com = comLoadA;
    devInformation.insert(2,loadAUart);
    if (loadA.isEmpty() || loadA.size() < 16) {
        ui->checkBox_A->setChecked(false);
    } else {
        ui->checkBox_A->setChecked(true);
    }

    QString loadB = CollectControl::HardSend(ui->lineEdit_LOADUART_B->text().right(1).toInt(), QString("RDM%101%102%\r").toLatin1().data(), 1000);
    devInformation.removeAt(3);
    UART_t loadBUart;
    loadBUart.data = loadB;
    comLoadB = ui->lineEdit_LOADUART_B->text().right(1).toInt();
    loadBUart.com = comLoadB;
    devInformation.insert(3,loadBUart);
    if (loadB.isEmpty() || loadB.size() < 16) {
        ui->checkBox_B->setChecked(false);
    } else {
        ui->checkBox_B->setChecked(true);
    }

    QString loadC = CollectControl::HardSend(ui->lineEdit_LOADUART_C->text().right(1).toInt(), QString("RDM%101%102%\r").toLatin1().data(), 1000);
    devInformation.removeAt(4);
    UART_t loadCUart;
    loadCUart.data = loadC;
    comLoadC = ui->lineEdit_LOADUART_C->text().right(1).toInt();
    loadCUart.com = comLoadC;
    devInformation.insert(4,loadCUart);
    if (loadC.isEmpty() || loadC.size() < 16) {
        ui->checkBox_C->setChecked(false);
    } else {
        ui->checkBox_C->setChecked(true);
    }

    QString testA = CollectControl::HardSend(ui->lineEdit_TESTUART->text().right(1).toInt(), QString("RDWD020603").toLatin1().data(), 1000);
    devInformation.removeAt(5);
    UART_t testAUart;
    testAUart.data = testA;
    comTest = ui->lineEdit_TESTUART->text().right(1).toInt();
    testAUart.com = comTest;
    devInformation.insert(5,testAUart);
    if (testA.isEmpty() || testA.size() != 13) {
        ui->checkBox_TEST->setChecked(false);
        ui->checkBox_TEST_A->setChecked(false);
    } else {
        ui->checkBox_TEST->setChecked(true);
        ui->checkBox_TEST_A->setChecked(true);
    }

    QString testB = CollectControl::HardSend(ui->lineEdit_TESTUART->text().right(1).toInt(), QString("RDWD030603").toLatin1().data(), 1000);
    devInformation.removeAt(6);
    UART_t testBUart;
    testBUart.data = testB;
    comTest = ui->lineEdit_TESTUART->text().right(1).toInt();
    testBUart.com = comTest;
    devInformation.insert(6,testBUart);
    if (testB.isEmpty() || testB.size() != 13) {
        ui->checkBox_TEST->setChecked(false);
        ui->checkBox_TEST_B->setChecked(false);
    } else {
        ui->checkBox_TEST->setChecked(true);
        ui->checkBox_TEST_B->setChecked(true);
    }

    QString testC = CollectControl::HardSend(ui->lineEdit_TESTUART->text().right(1).toInt(), QString("RDWD040603").toLatin1().data(), 1000);
    devInformation.removeAt(7);
    UART_t testCUart;
    testCUart.data = testC;
    comTest = ui->lineEdit_TESTUART->text().right(1).toInt();
    testCUart.com = comTest;
    devInformation.insert(7,testCUart);
    if (testC.isEmpty() || testC.size() != 13) {
        ui->checkBox_TEST->setChecked(false);
        ui->checkBox_TEST_C->setChecked(false);
    } else {
        ui->checkBox_TEST->setChecked(true);
        ui->checkBox_TEST_C->setChecked(true);
    }

    ui->tabWidget->setTabEnabled(0, true);
    ui->tabWidget->setTabEnabled(1, true);
    ui->tabWidget->setTabEnabled(2, true);
    ui->pushButton_4->setEnabled(false);

    QSettings *configIniWrite = new QSettings("EATconfig.ini", QSettings::IniFormat);
    configIniWrite->setValue("Voltage Uart", ui->lineEdit_VOLTUART->text().right(1));
    configIniWrite->setValue("Temperature Rise Uart", ui->lineEdit_TRCUART->text().right(1));
    configIniWrite->setValue("Load(ServoA) Uart", ui->lineEdit_LOADUART_A->text().right(1));
    configIniWrite->setValue("Load(ServoB) Uart", ui->lineEdit_LOADUART_B->text().right(1));
    configIniWrite->setValue("Load(ServoC) Uart", ui->lineEdit_LOADUART_C->text().right(1));
    configIniWrite->setValue("Life Tester Uart", ui->lineEdit_TESTUART->text().right(1));
    delete configIniWrite;

    uartTips();
}

void FormDisplay::on_checkBox_TEST_A_toggled(bool checked)
{
    Q_UNUSED(checked);

    if (devInformation.at(5).data.isEmpty() || devInformation.at(5).data.size() != 13){
        ui->checkBox_TEST_A->setChecked(false);
    } else {
        ui->checkBox_TEST_A->setChecked(true);
    }
}

void FormDisplay::on_checkBox_TEST_B_toggled(bool checked)
{
    Q_UNUSED(checked);

    if (devInformation.at(6).data.isEmpty() || devInformation.at(6).data.size() != 13){
        ui->checkBox_TEST_B->setChecked(false);
    } else {
        ui->checkBox_TEST_B->setChecked(true);
    }
}

void FormDisplay::on_checkBox_TEST_C_toggled(bool checked)
{
    Q_UNUSED(checked);

    if (devInformation.at(7).data.isEmpty() || devInformation.at(7).data.size() != 13){
        ui->checkBox_TEST_C->setChecked(false);
    } else {
        ui->checkBox_TEST_C->setChecked(true);
    }
}

int oldTabWidgetIndex = 0;
void FormDisplay::on_tabWidget_tabBarClicked(int index)
{
    if (oldTabWidgetIndex == 3) {
        if (ui->pushButton_4->isEnabled()) {
            QMessageBox msgBox(QMessageBox::Question,tr(" Electrical Accessories Test Automation Program"),tr("The UART data has changed, please update."));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setButtonText(QMessageBox::Ok,tr("Ok"));
            msgBox.setWindowIcon(QIcon(WINDOW_ICON));

            msgBox.exec();

            ui->tabWidget->setCurrentIndex(3);
            ui->tabWidget->setTabEnabled(0, false);
            ui->tabWidget->setTabEnabled(1, false);
            ui->tabWidget->setTabEnabled(2, false);
            oldTabWidgetIndex = index;
            return;
        } else {
            ui->tabWidget->setTabEnabled(0, true);
            ui->tabWidget->setTabEnabled(1, true);
            ui->tabWidget->setTabEnabled(2, true);
        }
    }

    oldTabWidgetIndex = index;
}

void FormDisplay::on_pushButton_5_clicked()
{
//    Collect c;
//    connectSTAS = c.ConnectToSTAS();

    CollectControl::HardSend(ui->lineEdit_VOLTUART->text().right(1).toInt(), QString("RDW VF").toLatin1().data(), 2000);
    QString volt = CollectControl::HardSend(ui->lineEdit_VOLTUART->text().right(1).toInt(), QString("RDW VF").toLatin1().data(), 2000);
    devInformation.removeAt(0);
    UART_t voltUart;
    voltUart.data = volt;
    voltUart.com = comVolt;
    devInformation.insert(0,voltUart);
    if (volt.isEmpty() || volt.size() < 14) {
        ui->checkBox_VOLT->setChecked(false);
    } else {
        ui->checkBox_VOLT->setChecked(true);
    }

    CollectControl::HardSend(ui->lineEdit_TRCUART->text().right(1).toInt(), QString("FData,0,0001,0110").toLatin1().data(), 2000);
    QString tc = CollectControl::HardSend(ui->lineEdit_TRCUART->text().right(1).toInt(), QString("FData,0,0001,0110").toLatin1().data(), 2000);
    devInformation.removeAt(1);
    UART_t trUart;
    trUart.data = tc;
    trUart.com = comTRC;
    devInformation.insert(1,trUart);
    if (tc.isEmpty() || tc.size() < 720) {
        ui->checkBox_TRC->setChecked(false);
    } else {
        ui->checkBox_TRC->setChecked(true);
    }

    CollectControl::HardSend(ui->lineEdit_LOADUART_A->text().right(1).toInt(), QString("RDM%101%102%\r").toLatin1().data(), 2000);
    QString loadA = CollectControl::HardSend(ui->lineEdit_LOADUART_A->text().right(1).toInt(), QString("RDM%101%102%\r").toLatin1().data(), 2000);
    devInformation.removeAt(2);
    UART_t loadAUart;
    loadAUart.data = loadA;
    loadAUart.com = comLoadA;
    devInformation.insert(2,loadAUart);
    if (loadA.isEmpty() || loadA.size() < 16) {
        ui->checkBox_A->setChecked(false);
    } else {
        ui->checkBox_A->setChecked(true);
    }

    CollectControl::HardSend(ui->lineEdit_LOADUART_B->text().right(1).toInt(), QString("RDM%101%102%\r").toLatin1().data(), 2000);
    QString loadB = CollectControl::HardSend(ui->lineEdit_LOADUART_B->text().right(1).toInt(), QString("RDM%101%102%\r").toLatin1().data(), 2000);
    devInformation.removeAt(3);
    UART_t loadBUart;
    loadBUart.data = loadB;
    loadBUart.com = comLoadB;
    devInformation.insert(3,loadBUart);
    if (loadB.isEmpty() || loadB.size() < 16) {
        ui->checkBox_B->setChecked(false);
    } else {
        ui->checkBox_B->setChecked(true);
    }

    CollectControl::HardSend(ui->lineEdit_LOADUART_C->text().right(1).toInt(), QString("RDM%101%102%\r").toLatin1().data(), 2000);
    QString loadC = CollectControl::HardSend(ui->lineEdit_LOADUART_C->text().right(1).toInt(), QString("RDM%101%102%\r").toLatin1().data(), 2000);
    devInformation.removeAt(4);
    UART_t loadCUart;
    loadCUart.data = loadC;
    loadCUart.com = comLoadC;
    devInformation.insert(4,loadCUart);
    if (loadC.isEmpty() || loadC.size() < 16) {
        ui->checkBox_C->setChecked(false);
    } else {
        ui->checkBox_C->setChecked(true);
    }

    CollectControl::HardSend(ui->lineEdit_TESTUART->text().right(1).toInt(), QString("RDWD020603").toLatin1().data(), 2000);
    QString testA = CollectControl::HardSend(ui->lineEdit_TESTUART->text().right(1).toInt(), QString("RDWD020603").toLatin1().data(), 2000);
    devInformation.removeAt(5);
    UART_t testAUart;
    testAUart.data = testA;
    testAUart.com = comTest;
    devInformation.insert(5,testAUart);
    if (testA.isEmpty() || testA.size() != 13) {
        ui->checkBox_TEST->setChecked(false);
        ui->checkBox_TEST_A->setChecked(false);
    } else {
        ui->checkBox_TEST->setChecked(true);
        ui->checkBox_TEST_A->setChecked(true);
    }

    QString testB = CollectControl::HardSend(ui->lineEdit_TESTUART->text().right(1).toInt(), QString("RDWD030603").toLatin1().data(), 2000);
    devInformation.removeAt(6);
    UART_t testBUart;
    testBUart.data = testB;
    testBUart.com = comTest;
    devInformation.insert(6,testBUart);
    if (testB.isEmpty() || testB.size() != 13) {
        ui->checkBox_TEST->setChecked(false);
        ui->checkBox_TEST_B->setChecked(false);
    } else {
        ui->checkBox_TEST->setChecked(true);
        ui->checkBox_TEST_B->setChecked(true);
    }

    QString testC = CollectControl::HardSend(ui->lineEdit_TESTUART->text().right(1).toInt(), QString("RDWD040603").toLatin1().data(), 2000);
    devInformation.removeAt(7);
    UART_t testCUart;
    testCUart.data = testC;
    testCUart.com = comTest;
    devInformation.insert(7,testCUart);
    if (testC.isEmpty() || testC.size() != 13) {
        ui->checkBox_TEST->setChecked(false);
        ui->checkBox_TEST_C->setChecked(false);
    } else {
        ui->checkBox_TEST->setChecked(true);
        ui->checkBox_TEST_C->setChecked(true);
    }
}

void FormDisplay::on_pushButton_6_clicked()
{
    ui->pushButton_6->menu()->exec();
}

void FormDisplay::on_lineEdit_tempMaxValue2_textChanged(const QString &arg1)
{
    QString str = arg1;
    QRegExp rxs("^[0-9]{0,2}$");
    if (!rxs.exactMatch(str)) {
        ui->lineEdit_tempMaxValue2->setText("1");
    }
    if (str.toInt() > 20) {
        ui->lineEdit_tempMaxValue2->setText("20");
    }
}

void FormDisplay::on_lineEdit_normalMaxValue2_textChanged(const QString &arg1)
{
    QString str = arg1;
    QRegExp rxs("^[0-9]{0,2}$");
    if (!rxs.exactMatch(str)) {
        ui->lineEdit_normalMaxValue2->setText("1");
    }
    if (str.toInt() > 20) {
        ui->lineEdit_normalMaxValue2->setText("20");
    }
}

void FormDisplay::on_lineEdit_tempMaxValue2_2_textChanged(const QString &arg1)
{
    QString str = arg1;
    QRegExp rxs("^[0-9]{0,2}$");
    if (!rxs.exactMatch(str)) {
        ui->lineEdit_tempMaxValue2_2->setText("1");
    }
    if (str.toInt() > 20) {
        ui->lineEdit_tempMaxValue2_2->setText("20");
    }
}

void FormDisplay::on_toolButton_clicked()
{
    m_helpWidget->show();
}

void FormDisplay::on_checkBox_temp_toggled(bool checked)
{
    if (checked) {
        ui->lineEdit_tempVn_t->setReadOnly(false);
        ui->lineEdit_tempIn_t->setReadOnly(false);
    } else {
        ui->lineEdit_tempVn_t->setReadOnly(true);
        ui->lineEdit_tempIn_t->setReadOnly(true);
    }
}

void FormDisplay::on_checkBox_normalTemp_toggled(bool checked)
{
    if (checked) {
        ui->lineEdit_normalTVn_t->setReadOnly(false);
        ui->lineEdit_normalTIn_t->setReadOnly(false);
    } else {
        ui->lineEdit_normalTVn_t->setReadOnly(true);
        ui->lineEdit_normalTIn_t->setReadOnly(true);
    }
}

void FormDisplay::on_checkBox_temp_2_toggled(bool checked)
{
    if (checked) {
        ui->lineEdit_tempVn_t_2->setReadOnly(false);
        ui->lineEdit_tempIn_t_2->setReadOnly(false);
    } else {
        ui->lineEdit_tempVn_t_2->setReadOnly(true);
        ui->lineEdit_tempIn_t_2->setReadOnly(true);
    }
}

void FormDisplay::on_checkBox_breaking_toggled(bool checked)
{
    if (checked) {
        ui->lineEdit_breakingVn_t->setReadOnly(false);
        ui->lineEdit_breakingIn_t->setReadOnly(false);
        ui->lineEdit_breaking_handleNum->setReadOnly(false);
    } else {
        ui->lineEdit_breakingVn_t->setReadOnly(true);
        ui->lineEdit_breakingIn_t->setReadOnly(true);
        ui->lineEdit_breaking_handleNum->setReadOnly(true);
    }
}

void FormDisplay::on_checkBox_normal_toggled(bool checked)
{
    if (checked) {
        ui->lineEdit_normalVn_t->setReadOnly(false);
        ui->lineEdit_normalIn_t->setReadOnly(false);
        ui->lineEdit_normal_handleNum->setReadOnly(false);
    } else {
        ui->lineEdit_normalVn_t->setReadOnly(true);
        ui->lineEdit_normalIn_t->setReadOnly(true);
        ui->lineEdit_normal_handleNum->setReadOnly(true);
    }
}

void FormDisplay::on_checkBox_breaking_2_toggled(bool checked)
{
    if (checked) {
        ui->lineEdit_breakingVn_t_2->setReadOnly(false);
        ui->lineEdit_breakingIn_t_2->setReadOnly(false);
        ui->lineEdit_breaking_handleNum_2->setReadOnly(false);
    } else {
        ui->lineEdit_breakingVn_t_2->setReadOnly(true);
        ui->lineEdit_breakingIn_t_2->setReadOnly(true);
        ui->lineEdit_breaking_handleNum_2->setReadOnly(true);
    }
}

void FormDisplay::on_checkBox_normal_2_toggled(bool checked)
{
    if (checked) {
        ui->lineEdit_normalVn_t_2->setReadOnly(false);
        ui->lineEdit_normalIn_t_2->setReadOnly(false);
        ui->lineEdit_normal_handleNum_2->setReadOnly(false);
    } else {
        ui->lineEdit_normalVn_t_2->setReadOnly(true);
        ui->lineEdit_normalIn_t_2->setReadOnly(true);
        ui->lineEdit_normal_handleNum_2->setReadOnly(true);
    }
}
