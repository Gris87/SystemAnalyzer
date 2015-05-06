#include "executorthread.h"

#include <QSettings>
#include <QDebug>

#include "rules.h"



#define STEP_INITIALIZATION() \
    { \
        if (mTerminated)            \
        {                           \
            return;                 \
        }                           \
                                    \
        mBaseProgress = mProgress;  \
    }

#define CHECK_AUTORUN_STEP(function, step, stepsCount) \
    {                                           \
        if (mTerminated)                            \
        {                                           \
            return;                                 \
        }                                           \
                                                    \
        function();                                 \
                                                    \
        ++step;                                     \
        updateProgress(step * 100 / stepsCount);    \
    }



ExecutorThread::ExecutorThread(Rules *rules, QObject *parent)
    : QThread(parent)
    , mTerminated(false)
    , mRules(rules)
    , mStepsCount(0)
    , mBaseProgress(0)
    , mProgress(0)
    , mError(false)
{
}

ExecutorThread::~ExecutorThread()
{
}

void ExecutorThread::stop()
{
    mTerminated = true;
}

bool ExecutorThread::isError()
{
    return mError;
}

void ExecutorThread::run()
{
    if (mRules->isCheckAutorun())
    {
        ++mStepsCount;
    }

    if (mRules->isCheckSystemFiles())
    {
        ++mStepsCount;
    }

    // ----------------------------------------------------

    if (mRules->isCheckAutorun())
    {
        checkAutorun();
    }

    if (mRules->isCheckSystemFiles())
    {
        checkSystemFiles();
    }
}

void ExecutorThread::updateProgress(quint8 progress)
{
    Q_ASSERT(progress <= 100);

    progress = mBaseProgress + progress / mStepsCount;

    if (mProgress != progress)
    {
        Q_ASSERT(progress > mProgress);
        mProgress = progress;

        emit progressChanged(mProgress);
    }
}

void ExecutorThread::checkAutorun()
{
    STEP_INITIALIZATION();

    quint8 step = 0;

    // Windows
#ifdef Q_OS_WIN
    {
        quint8 stepsCount = 5;

        CHECK_AUTORUN_STEP(checkAutorun_HKCU_Run,       step, stepsCount);
        CHECK_AUTORUN_STEP(checkAutorun_HKCU_RunOnce,   step, stepsCount);
        CHECK_AUTORUN_STEP(checkAutorun_HKLM_Run,       step, stepsCount);
        CHECK_AUTORUN_STEP(checkAutorun_HKLM_RunOnce,   step, stepsCount);
        CHECK_AUTORUN_STEP(checkAutorun_StartUp,        step, stepsCount);
    }
#endif
}

#ifdef Q_OS_WIN
void ExecutorThread::checkAutorun_VerifyRegistry(const QString &regKey)
{
    qDebug() << regKey;

    QSettings settings(regKey, QSettings::NativeFormat);

    QStringList keys = settings.allKeys();

    for (int i = 0; i < keys.length(); ++i)
    {
        const QString &key = keys.at(i);
        QString value = settings.value(key).toString();

        qDebug() << key << " = " << value;
    }

    // TODO: Save to file and compare
}

void ExecutorThread::checkAutorun_HKCU_Run()
{
    checkAutorun_VerifyRegistry("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
}

void ExecutorThread::checkAutorun_HKCU_RunOnce()
{
    checkAutorun_VerifyRegistry("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce");
}

void ExecutorThread::checkAutorun_HKLM_Run()
{
    checkAutorun_VerifyRegistry("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
}

void ExecutorThread::checkAutorun_HKLM_RunOnce()
{
    checkAutorun_VerifyRegistry("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce");
}

void ExecutorThread::checkAutorun_StartUp()
{
    // TODO: Implement ExecutorThread::checkAutorun_StartUp
}
#endif

void ExecutorThread::checkSystemFiles()
{
    STEP_INITIALIZATION();

    updateProgress(25);
    QThread::msleep(1000);

    updateProgress(50);
    QThread::msleep(1000);

    updateProgress(75);
    QThread::msleep(1000);

    updateProgress(100);
    QThread::msleep(1000);

    // TODO: Implement ExecutorThread::checkSystemFiles
}
