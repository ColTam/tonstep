#ifndef MTRCRTHREAD_H
#define MTRCRTHREAD_H

#include <QObject>

#ifdef _MSC_BUILD
#pragma execution_character_set("utf-8")
#endif

class QTimer;

/*!
 * \brief The temperature rise thread class
 */
class mTRCRThread : public QObject
{
    Q_OBJECT
public:
    explicit mTRCRThread(QObject *parent = 0);
    ~mTRCRThread();

    bool mtRCRIsActive();       /*!* \brief 返回定时器是否启动 */

signals:
    void tRCRData(QStringList); /*!* \brief 温升数据更新信号 */

public slots:
    void readData();            /*!* \brief 线程初始化槽函数 */
    void updaeTRCR();           /*!* \brief 接收无纸记录仪数据槽函数 */
    void mTRCRTimerStart();     /*!* \brief 定时器启动 */
    void mTRCRTimerStop();      /*!* \brief 定时器停止 */

private:
    QTimer* _trTimer;           /*!* \brief 定时器对象 */

    QString oldTc;              /*!* \brief 旧温升数据 */
    QStringList readList;       /*!* \brief 数据list */
};

#endif // MTRCRTHREAD_H
