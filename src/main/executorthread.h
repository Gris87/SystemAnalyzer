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

    bool isError();

protected:
    void run();

private:
    void updateProgress(quint8 progress);

    void checkAutorun();
#ifdef Q_OS_WIN
    void checkAutorun_VerifyRegistry(const QString &regKey);
    void checkAutorun_HKCU_Run();
    void checkAutorun_HKCU_RunOnce();
    void checkAutorun_HKLM_Run();
    void checkAutorun_HKLM_RunOnce();
    void checkAutorun_StartUp();
#endif

    void checkSystemFiles();

    bool   mTerminated;

    Rules *mRules;

    quint8 mStepsCount;
    quint8 mBaseProgress;
    quint8 mProgress;
    bool   mError;

signals:
    void progressChanged(quint8 progress);
};

#endif // EXECUTORTHREAD_H
