#include "warningdialog.h"
#include "ui_warningdialog.h"
#include "titlebar.h"

#include <QPropertyAnimation>
#include <QVBoxLayout>
#include <QSound>

WarningDialog::WarningDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WarningDialog)
{
    ui->setupUi(this);

    initDialogTitle();
//    bells = new QSound(":/sound/sound/beep.wav");
//    bells->setLoops(-1);//无限循环播放
//    bells->play();
}

WarningDialog::~WarningDialog()
{
    delete ui;
//    delete bells;
}

void WarningDialog::initDialogTitle()
{
    this->setWindowFlags(Qt::FramelessWindowHint);

    TitleBar *titleBar = new TitleBar(this);
    this->installEventFilter(titleBar);

    this->setWindowTitle(" Electrical Accessories Test Automation Program-Warning Widget");
    this->setWindowIcon(QIcon(":/icon/icon/5"));

    QPalette mPalette(palette());
    mPalette.setColor(QPalette::Background, QColor(64,66,68));
    this->setAutoFillBackground(true);
    this->setPalette(mPalette);

    QVBoxLayout *titleVBoxLayout = new QVBoxLayout();
    titleVBoxLayout->addWidget(titleBar);
    titleVBoxLayout->addStretch(0);
    titleVBoxLayout->setSpacing(0);
    titleVBoxLayout->setContentsMargins(0, 0, 0, 0);

    connect(titleBar, SIGNAL(closeClicked()), this, SLOT(closeDialog()));

    this->setLayout(titleVBoxLayout);
}

void WarningDialog::setText(QString text)
{
    ui->label_text->setText(text);
}

void WarningDialog::closeDialog()
{
//    bells->stop();
//    bells->destroyed();

    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(618);
    animation->setStartValue(1);
    animation->setEndValue(0);
    animation->start();

    connect(animation, SIGNAL(finished()), this, SLOT(close()));
}

void WarningDialog::on_pushButton_clicked()
{
    this->closeDialog();
}
