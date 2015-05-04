#include "executorthread.h"

ExecutorThread::ExecutorThread(Rules *rules, QObject *parent)
    : QThread(parent)
    , mTerminated(false)
    , mRules(rules)
{
}

ExecutorThread::~ExecutorThread()
{
}

void ExecutorThread::stop()
{
    mTerminated = true;
}

void ExecutorThread::run()
{
    // TODO: Implement ExecutorThread::run

    for (int i = 0; i < 100 && !mTerminated; ++i)
    {
        emit progressChanged(i);

        QThread::msleep(100);
    }
}
