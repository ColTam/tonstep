#ifndef HELPWIDGET_H
#define HELPWIDGET_H

#include <QWidget>

#ifdef _MSC_BUILD
#pragma execution_character_set("utf-8")
#endif

class QFrame;
class QLabel;
class QToolButton;
/*!
 * \brief The help widget class
 */
class HelpWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HelpWidget(QWidget *parent = 0);

public:
    void initForm();            /*!* \brief  */
    void initWidget();          /*!* \brief  */
    void initConnect();         /*!* \brief  */

signals:

public slots:

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void changeFrame(int flag);

private:
    QFrame* m_frame;            /*!* \brief 框架 */
//    QLabel* m_toRunLabel;       /*!* \brief 待命框说明 */
//    QLabel* m_toHaveLabel;      /*!* \brief 备选框说明 */
//    QLabel* m_toStartLabel;     /*!* \brief 开始按钮说明 */
//    QLabel* m_toServoLabel;     /*!* \brief 伺服选择说明 */
//    QLabel* m_toDawdlerLabel;   /*!* \brief 懒人模式说明 */
//    QLabel* m_toFunctionLabel;  /*!* \brief 功能流程说明 */
    QToolButton *m_lastPageButton;
    QToolButton *m_nextPageButton;
    QToolButton *m_closePageButton;

    int num;
};

#endif // HELPWIDGET_H
