﻿#include "minidatewidget.h"

#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>

MiniDateWidget::MiniDateWidget(QWidget *parent) : QWidget(parent)
  ,isPressed(0)
{
    resize(196, 121);
    setAutoFillBackground(true);
    setStyleSheet("background: rgba(20, 20, 20, 30%)");

    groupbox = new QGroupBox(this);
    groupbox->setGeometry(4, 1, 188, 117);
    groupbox->setTitle(tr(" Electrical Accessories Test Automation Program"));
    groupbox->setStyleSheet("color: white");

    QLabel *overTimeLabel = new QLabel(this);
    QLabel *voltLabel = new QLabel(this);
    QLabel *LPFLabel = new QLabel(this);
    QLabel *currentLabel = new QLabel(this);
    overTimeLineEdit = new QLineEdit(this);
    voltLineEdit = new QLineEdit(this);
    LPFLineEdit = new QLineEdit(this);
    currentLineEdit = new QLineEdit(this);

    overTimeLabel->setGeometry(8, 16, 100, 20);
    voltLabel->setGeometry(8, 41, 100, 20);
    LPFLabel->setGeometry(8, 66, 100, 20);
    currentLabel->setGeometry(8, 91, 100, 20);
    overTimeLineEdit->setGeometry(116, 16, 71, 20);
    voltLineEdit->setGeometry(116, 41, 71, 20);
    LPFLineEdit->setGeometry(116, 66, 71, 20);
    currentLineEdit->setGeometry(116, 91, 71, 20);

    overTimeLabel->setText(tr("FinishTime :"));
    voltLabel->setText(tr("Voltage :"));
    LPFLabel->setText(tr("Load&PowerFactor:"));
    currentLabel->setText(tr("Current :"));

    overTimeLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    voltLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    LPFLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    currentLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    overTimeLineEdit->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    voltLineEdit->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    LPFLineEdit->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    currentLineEdit->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    overTimeLabel->setStyleSheet("background: transparent; color: white");
    voltLabel->setStyleSheet("background: transparent; color: white");
    LPFLabel->setStyleSheet("background: transparent; color: white");
    currentLabel->setStyleSheet("background: transparent; color: white");
    overTimeLineEdit->setStyleSheet("color: white");
    voltLineEdit->setStyleSheet("color: white");
    LPFLineEdit->setStyleSheet("color: white");
    currentLineEdit->setStyleSheet("color: white");

    overTimeLineEdit->setReadOnly(true);
    voltLineEdit->setReadOnly(true);
    LPFLineEdit->setReadOnly(true);
    currentLineEdit->setReadOnly(true);
}

MiniDateWidget::~MiniDateWidget()
{
    delete currentLineEdit;
    delete LPFLineEdit;
    delete voltLineEdit;
    delete overTimeLineEdit;
    delete groupbox;
}

void MiniDateWidget::setText(QString volt, QString current, QString LPF)
{
    voltLineEdit->setText(volt + " V");
    currentLineEdit->setText(current + " A");
    LPFLineEdit->setText(LPF);
}

void MiniDateWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isPressed = true;
        mPoint = event->pos();
    } else {
        event->accept();
    }
}

void MiniDateWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (isPressed) {
        move(event->pos() - mPoint + pos());
    } else {
        event->accept();
    }
}

void MiniDateWidget::mouseReleaseEvent(QMouseEvent *event)
{
    isPressed = false;

    if (pos().x() < 0) {
        move(0, pos().y());
    }

    if (pos().y() < 101) {
        move(pos().x(), 101);
    }

    if (pos().x() > this->parentWidget()->width() - this->width()) {
        //windows.width() - 2 * this.x() - this.width()
        move(this->parentWidget()->width() - this->width(), pos().y());
    }//1024 => 828 | 800 => 604

    if (pos().y() > this->parentWidget()->height() - this->height()) {
        //768 => 647 | 600 => 479
        move(pos().x(), this->parentWidget()->height() - this->height());
    }

    event->accept();
}
