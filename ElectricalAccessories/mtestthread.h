#ifndef MTESTTHREAD_H
#define MTESTTHREAD_H

#include <QObject>

#ifdef _MSC_BUILD
#pragma execution_character_set("utf-8")
#endif

class QTimer;

/*!
 * \brief The life tester thread class
 */
class mTestThread : public QObject
{
    Q_OBJECT
public:
    explicit mTestThread(QObject *parent = 0);
    ~mTestThread();

signals:
    void testDataA(QString);/*!* \brief 伺服数据信号 */
    void testDataB(QString);/*!* \brief  */
    void testDataC(QString);/*!* \brief  */

public slots:
    void readData();        /*!* \brief 线程初始化槽函数 */
    void updaeTest();       /*!* \brief 接收寿命测试机数据槽函数 */
    void mTestTimerStart(); /*!* \brief 定时器启动 */
    void mTestTimerStop();  /*!* \brief 定时器停止 */

private:
    QTimer* _testTimer;     /*!* \brief 定时器对象 */

    QString oldNumberA;     /*!* \brief 旧寿命测试机数据 */
    QString oldNumberB;     /*!* \brief  */
    QString oldNumberC;     /*!* \brief  */

    int flag = 0;           /*!* \brief 读取某伺服标识 */
};

#endif // MTESTTHREAD_H
