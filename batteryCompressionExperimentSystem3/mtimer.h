#ifndef MTIMER_H
#define MTIMER_H

#include <QObject>

//#ifdef _MSC_BUILD
//#pragma execution_character_set("utf-8")
//#endif

class QTimer;

class mTimer : public QObject
{
    Q_OBJECT
public:
    explicit mTimer(QObject *parent = 0);
    ~mTimer();

signals:

public Q_SLOTS:
//    void mTimerStart();
//    void mTimerStop();

private Q_SLOTS:
    void initThread();
    void mTimerout();

private:
    QTimer *m_timer;
};

#endif // MTIMER_H
