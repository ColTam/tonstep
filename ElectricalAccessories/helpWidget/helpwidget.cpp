#include "helpwidget.h"

#include <QLabel>
#include <QMouseEvent>
#include <QToolButton>

HelpWidget::HelpWidget(QWidget *parent) : QWidget(parent)
  , num(1)
{
    this->initForm();
    this->initWidget();
    this->initConnect();
}

void HelpWidget::initForm()
{
    setFixedSize(943, 633);
//    setAutoFillBackground(true);
    setStyleSheet("background: rgba(50, 50, 70, 20%)");
}

void HelpWidget::initWidget()
{
    m_frame = new QFrame(this);
    m_frame->setGeometry(0, 0, this->width(), this->height());
    m_frame->setStyleSheet("QFrame {border-image: url(:/image/image/help);}"
                           "QFrame:: {}");

//    m_toRunLabel = new QLabel(this);
//    m_toRunLabel->setGeometry(300, 70, 300, 60);
//    m_toRunLabel->setStyleSheet(LABELSTYLESHEET);

//    m_toHaveLabel = new QLabel(this);
//    m_toHaveLabel->setGeometry(300, 210, 300, 60);
//    m_toHaveLabel->setStyleSheet(LABELSTYLESHEET);

//    m_toStartLabel = new QLabel(this);
//    m_toStartLabel->setGeometry(800, 230, 130, 60);
//    m_toStartLabel->setStyleSheet(LABELSTYLESHEET);

//    m_toServoLabel = new QLabel(this);
//    m_toServoLabel->setGeometry(740, 70, 190, 50);
//    m_toServoLabel->setStyleSheet(LABELSTYLESHEET);

//    m_toDawdlerLabel = new QLabel(this);
//    m_toDawdlerLabel->setGeometry(800, 180, 130, 40);
//    m_toDawdlerLabel->setStyleSheet(LABELSTYLESHEET);

//    m_toFunctionLabel = new QLabel(this);
//    m_toFunctionLabel->setGeometry(130, 330, 650, 270);
//    m_toFunctionLabel->setStyleSheet(LABELSTYLESHEET);

    m_lastPageButton = new QToolButton(m_frame);
    m_lastPageButton->setText(tr("<"));
    m_lastPageButton->setGeometry(m_frame->width()-55,0, 26, 26);
    m_lastPageButton->setAutoRaise(true);
    m_lastPageButton->setStyleSheet("border:none; background:transparent;");
    m_lastPageButton->setEnabled(false);
    m_nextPageButton = new QToolButton(m_frame);
    m_nextPageButton->setText(tr(">"));
    m_nextPageButton->setGeometry(m_frame->width()-26,0, 26, 26);
    m_nextPageButton->setAutoRaise(true);
    m_nextPageButton->setStyleSheet("border:none; background:transparent;");
}

void HelpWidget::initConnect()
{
    connect(m_lastPageButton, &QToolButton::clicked, [this]() {
        changeFrame(--num);
    });
    connect(m_nextPageButton, &QToolButton::clicked, [this]() {
        changeFrame(++num);
    });
}

void HelpWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        this->hide();
        num = 1;
        changeFrame(num);
    }

    QWidget::mousePressEvent(event);
}

void HelpWidget::changeFrame(int flag)
{
    switch (flag) {
    case 1:
        m_lastPageButton->setEnabled(false);
        m_nextPageButton->setEnabled(true);

        m_frame->setStyleSheet("QFrame {border-image: url(:/image/image/help);}"
                               "QFrame:: {}");
        break;
    case 2:
        m_lastPageButton->setEnabled(true);
        m_nextPageButton->setEnabled(true);

        m_frame->setStyleSheet("QFrame {border-image: url(:/image/image/help_first);}"
                               "QFrame:: {}");
        break;
    case 3:
        m_lastPageButton->setEnabled(true);
        m_nextPageButton->setEnabled(true);

        m_frame->setStyleSheet("QFrame {border-image: url(:/image/image/help_second);}"
                               "QFrame:: {}");
        break;
    case 4:
        m_lastPageButton->setEnabled(true);
        m_nextPageButton->setEnabled(false);

        m_frame->setStyleSheet("QFrame {border-image: url(:/image/image/help_thrid);}"
                               "QFrame:: {}");
        break;
    default:
        break;
    }
}
