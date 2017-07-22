#ifndef MUPDATETHREAD_H
#define MUPDATETHREAD_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QVector>

class mUpdateThread : public QObject
{
    Q_OBJECT
    QMutex mutex;
    QStringList _state;
public:
    explicit mUpdateThread(QObject *parent = 0);

signals:
    void updatedState();

public slots:
    void updateState();
};

class mUpdateController : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    mUpdateController(){
        mUpdateThread *worker = new mUpdateThread;
        worker->moveToThread(&workerThread);

        connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
        connect(this, &mUpdateController::updateUart, worker, &mUpdateThread::updateState);
        connect(worker, &mUpdateThread::updatedState, this, &mUpdateController::getUpdateUart);

        workerThread.start();
    }
    ~mUpdateController() {
        workerThread.quit();
        workerThread.wait();
    }
signals:
    void updateUart();
    void getUpdateUart();
};

#endif // MUPDATETHREAD_H
