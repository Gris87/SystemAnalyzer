#include "rules.h"

#include <QDate>



Rules::Rules(QObject *parent)
    : QObject(parent)
    , mType(SCHEDULE_TYPE_EACH_MINUTES)
    , mEachMinutes(1)
    , mDays(SCHEDULE_DAYS_ALL)
    , mCheckAutorun(true)
    , mCheckSystemFiles(true)
    , mProgressBar(0)
    , mExecutorThread(0)
    , mNeedRestart(false)
{
}

Rules::~Rules()
{
    if (isRunning())
    {
        stop();
    }
}

void Rules::start()
{
    Q_ASSERT(mExecutorThread == 0);

    mNeedRestart = false;

    mExecutorThread = new ExecutorThread(this, this);

    connect(mExecutorThread, SIGNAL(progressChanged(int)), this, SLOT(progressChanged(int)), Qt::BlockingQueuedConnection);
    connect(mExecutorThread, SIGNAL(finished()),           this, SLOT(executorThreadFinished()));

    mExecutorThread->start(QThread::LowestPriority);
}

void Rules::stop()
{
    Q_ASSERT(mExecutorThread);

    // TODO: Stop timer

    mNeedRestart = false;

    disconnect(mExecutorThread, SIGNAL(progressChanged(int)), this, SLOT(progressChanged(int)));
    disconnect(mExecutorThread, SIGNAL(finished()),           this, SLOT(executorThreadFinished()));

    mExecutorThread->stop();
    mExecutorThread->wait();

    executorThreadFinished();
}

void Rules::checkIfNeedStart()
{
    switch (mType)
    {
        case SCHEDULE_TYPE_MANUALLY:
        {
            // Nothing
        }
        break;

        case SCHEDULE_TYPE_EACH_MINUTES:
        {
            // TODO: Need to get time of next start and initiate timer
            if (isRunning())
            {
                mNeedRestart = true;
            }
            else
            {
                start();
            }
        }
        break;

        case SCHEDULE_TYPE_DAYS:
        {
            // TODO: Need to get time of next start and initiate timer
            if (mDays & (1 << (QDate::currentDate().dayOfWeek() - 1)))
            {
                if (isRunning())
                {
                    mNeedRestart = true;
                }
                else
                {
                    start();
                }
            }
        }
        break;

        default:
        {
            qFatal("Unknown schedule type");
        }
        break;
    }
}

void Rules::reset()
{
    mType             = SCHEDULE_TYPE_EACH_MINUTES;
    mEachMinutes      = 1;
    mDays             = SCHEDULE_DAYS_ALL;
    mCheckAutorun     = true;
    mCheckSystemFiles = true;
}

const QString Rules::toString() const
{
    switch (mType)
    {
        case SCHEDULE_TYPE_MANUALLY:       return tr("Manually");
        case SCHEDULE_TYPE_EACH_MINUTES:   return tr("Each %1 minute(s)").arg(mEachMinutes);
        case SCHEDULE_TYPE_DAYS:
        {
            QString res;

            if (mDays & SCHEDULE_DAYS_MONDAY)
            {
                if (res != "")
                {
                    res.append(";");
                }

                res.append(tr("Mon"));
            }

            if (mDays & SCHEDULE_DAYS_TUESDAY)
            {
                if (res != "")
                {
                    res.append(";");
                }

                res.append(tr("Tue"));
            }

            if (mDays & SCHEDULE_DAYS_WEDNESDAY)
            {
                if (res != "")
                {
                    res.append(";");
                }

                res.append(tr("Wed"));
            }

            if (mDays & SCHEDULE_DAYS_THURSDAY)
            {
                if (res != "")
                {
                    res.append(";");
                }

                res.append(tr("Thu"));
            }

            if (mDays & SCHEDULE_DAYS_FRIDAY)
            {
                if (res != "")
                {
                    res.append(";");
                }

                res.append(tr("Fri"));
            }

            if (mDays & SCHEDULE_DAYS_SATURDAY)
            {
                if (res != "")
                {
                    res.append(";");
                }

                res.append(tr("Sat"));
            }

            if (mDays & SCHEDULE_DAYS_SUNDAY)
            {
                if (res != "")
                {
                    res.append(";");
                }

                res.append(tr("Sun"));
            }

            if (res == "")
            {
                res = tr("Manually");
            }

            return res;
        }

        default: return "UNKNOWN SCHEDULE TYPE";
    }
}

Rules::ScheduleType Rules::getType() const
{
    return mType;
}

void Rules::setType(Rules::ScheduleType value)
{
    mType = value;
}

quint16 Rules::getEachMinutes() const
{
    return mEachMinutes;
}

quint8 Rules::getDaysMask() const
{
    return mDays;
}

void Rules::setEachMinutes(quint16 value)
{
    if (value < 1)
    {
        value = 1;
    }

    mEachMinutes = value;
}

bool Rules::isMondayEnabled() const
{
    return mDays & SCHEDULE_DAYS_MONDAY;
}

void Rules::setMondayEnabled(bool value)
{
    if (value)
    {
        mDays |= SCHEDULE_DAYS_MONDAY;
    }
    else
    {
        mDays &= ~SCHEDULE_DAYS_MONDAY;
    }
}

bool Rules::isTuesdayEnabled() const
{
    return mDays & SCHEDULE_DAYS_TUESDAY;
}

void Rules::setTuesdayEnabled(bool value)
{
    if (value)
    {
        mDays |= SCHEDULE_DAYS_TUESDAY;
    }
    else
    {
        mDays &= ~SCHEDULE_DAYS_TUESDAY;
    }
}

bool Rules::isWednesdayEnabled() const
{
    return mDays & SCHEDULE_DAYS_WEDNESDAY;
}

void Rules::setWednesdayEnabled(bool value)
{
    if (value)
    {
        mDays |= SCHEDULE_DAYS_WEDNESDAY;
    }
    else
    {
        mDays &= ~SCHEDULE_DAYS_WEDNESDAY;
    }
}

bool Rules::isThursdayEnabled() const
{
    return mDays & SCHEDULE_DAYS_THURSDAY;
}

void Rules::setThursdayEnabled(bool value)
{
    if (value)
    {
        mDays |= SCHEDULE_DAYS_THURSDAY;
    }
    else
    {
        mDays &= ~SCHEDULE_DAYS_THURSDAY;
    }
}

bool Rules::isFridayEnabled() const
{
    return mDays & SCHEDULE_DAYS_FRIDAY;
}

void Rules::setFridayEnabled(bool value)
{
    if (value)
    {
        mDays |= SCHEDULE_DAYS_FRIDAY;
    }
    else
    {
        mDays &= ~SCHEDULE_DAYS_FRIDAY;
    }
}

bool Rules::isSaturdayEnabled() const
{
    return mDays & SCHEDULE_DAYS_SATURDAY;
}

void Rules::setSaturdayEnabled(bool value)
{
    if (value)
    {
        mDays |= SCHEDULE_DAYS_SATURDAY;
    }
    else
    {
        mDays &= ~SCHEDULE_DAYS_SATURDAY;
    }
}

bool Rules::isSundayEnabled() const
{
    return mDays & SCHEDULE_DAYS_SUNDAY;
}

void Rules::setSundayEnabled(bool value)
{
    if (value)
    {
        mDays |= SCHEDULE_DAYS_SUNDAY;
    }
    else
    {
        mDays &= ~SCHEDULE_DAYS_SUNDAY;
    }
}

bool Rules::isCheckAutorun() const
{
    return mCheckAutorun;
}

void Rules::setCheckAutorun(bool value)
{
    mCheckAutorun = value;
}

bool Rules::isCheckSystemFiles() const
{
    return mCheckSystemFiles;
}

void Rules::setCheckSystemFiles(bool value)
{
    mCheckSystemFiles = value;
}

void Rules::setProgressBar(QProgressBar *progressBar)
{
    mProgressBar = progressBar;
}

bool Rules::isRunning() const
{
    return mExecutorThread != 0;
}

void Rules::progressChanged(int progress)
{
    Q_ASSERT(mProgressBar);

    mProgressBar->setValue(progress);
}

void Rules::executorThreadFinished()
{
    mProgressBar->setValue(0);

    delete mExecutorThread;
    mExecutorThread = 0;    

    if (mNeedRestart)
    {
        start();
    }
}
