#ifndef MLOADTHREAD_H
#define MLOADTHREAD_H

#include <QObject>

#pragma execution_character_set("utf-8")

class QTimer;

/*!
 * \brief The load thread class
 */
class mLoadThread : public QObject
{
    Q_OBJECT
public:
    explicit mLoadThread(QObject *parent = 0);
    ~mLoadThread();

    bool    isFirst;                                        /*!* \brief 判断首次收到的有效数据 */
    QString judgeVoltage(double v1, double v2);             /*!* \brief 优化电压数据 */
    QString judgeCurrent(double c1, double c2);             /*!* \brief 优化电流数据 */
    QString judgeLPF(double l1, double l2);                 /*!* \brief 优化功率因素数据 */
    QString dataControl(QString data, double c, double pf); /*!* \brief 数据控制 */

signals:
    void updateData(QString,QString);                       /*!* \brief 数据更新信号 */
    void changeVoltage(QString);                            /*!* \brief 电压需要微调信号 */

public slots:
    void loadServo(QString servo);                          /*!* \brief 接收负载对应伺服号槽函数 */
    void readData();                                        /*!* \brief 线程初始化槽函数 */
    void updateData();                                      /*!* \brief 接受负载数据 */
    void mLoadTimerStart(int time);                         /*!* \brief 定时器启动 */
    void mLoadTimerStop();                                  /*!* \brief 定时器停止 */

private:
    QTimer* _loadTimer;                                     /*!* \brief 线程定时器 */
    time_t  _time;                                          /*!* \brief 定时器延时 */
    QString servo;                                          /*!* \brief 项目伺服号 */
    QString volt;                                           /*!* \brief 接收的电压数据 */
    QString load;                                           /*!* \brief 接收的负载数据 */
    QString oldVole;                                        /*!* \brief 旧电压数据 */
    QString oldLoadA;                                       /*!* \brief 旧负载数据 */
    QString oldLoadB;                                       /*!* \brief  */
    QString oldLoadC;                                       /*!* \brief  */

    double  trLoadU;                                         /*!* \brief 负载微调幅度 */
    double  trLoadD;                                         /*!* \brief 负载微调幅度 */
    bool    isOn;                                           /*!* \brief 判断此次数据是否为有效通值 */
    bool    isOff;                                          /*!* \brief 判断此次数据是否为有效断值 */
};

#endif // MLOADTHREAD_H
