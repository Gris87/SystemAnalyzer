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

    bool isReportCreated();

protected:
    void run();

private:
    void updateProgress(quint8 progress);
    void compareAndStore(const QByteArray &cachedArray, const QByteArray &newArray, const QString &category, const QString &fileName);

    void checkAutorun();
#ifdef Q_OS_WIN
    void checkAutorun_VerifyRegistry(const QString &regKey, const QString &fileName);
    void checkAutorun_HKCU_Run();
    void checkAutorun_HKCU_RunOnce();
    void checkAutorun_HKLM_Run();
    void checkAutorun_HKLM_RunOnce();
    void checkAutorun_StartUpVerifyRegistry(const QString &regKey, const QString &key, const QString &fileName);
    void checkAutorun_StartUp_User1();
    void checkAutorun_StartUp_User2();
    void checkAutorun_StartUp_Common1();
    void checkAutorun_StartUp_Common2();
#endif

    void checkSystemFiles();

    bool   mTerminated;

    Rules *mRules;

    QString mTimestamp;
    quint8  mStepsCount;
    quint8  mBaseProgress;
    quint8  mProgress;
    bool    mReportCreated;

signals:
    void progressChanged(quint8 progress);
};

#endif // EXECUTORTHREAD_H
