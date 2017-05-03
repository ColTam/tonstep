#ifndef QMESSAGEWIDGET_H
#define QMESSAGEWIDGET_H

#define MESSAGE_HEIGHT                  26
#define MESSAGE_ICON_WIDTH              33
#define MESSAGE_BUTTON_WIDTH            42
#define MESSAGE_STYLE                   "background: rgba(50, 50, 50, 20%)"
#define MESSAGE_TEXT_STYLE              "color: rgb(0, 120, 215)"
#define MESSAGE_CLOSEBUTTON_STYLE       "QToolButton#closeTipButton:hover   {color: rgb(192, 0, 0)}"\
                                        "QToolButton#closeTipButton:pressed {color: rgb(225,127,127)}"

#include <QHBoxLayout>
#include <QToolButton>
#include <QWidget>

#pragma execution_character_set("utf-8")

class QLabel;
class QPushButton;

/*!
 * \brief The message widget class
 */
class QMessageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QMessageWidget(QWidget *parent = 0);
    ~QMessageWidget();

    void setText(const QString &text);/*!* \brief 设置界面显示文本 */

private:
    QLabel      *pIconLabel;        /*!* \brief 界面图标便签 */
    QToolButton *pCloseButton;      /*!* \brief 界面关闭按钮 */
    QLabel      *mMessageLabel;     /*!* \brief 文本显示便签 */
    QHBoxLayout *pLayout;           /*!* \brief 水平布局对象 */
};

#endif // QMESSAGEWIDGET_H
