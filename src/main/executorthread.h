#ifndef EXECUTORTHREAD_H
#define EXECUTORTHREAD_H

#include <QThread>



class Rules;



class ExecutorThread : public QThread
{
    Q_OBJECT

public:
    ExecutorThread(Rules *rules, QObject *parent = 0);
    ~ExecutorThread();

    void stop();

protected:
    void run();

private:
    bool   mTerminated;

    Rules *mRules;

signals:
    void progressChanged(int progress);
};

#endif // EXECUTORTHREAD_H
