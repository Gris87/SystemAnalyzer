#include "rules.h"

#include <QApplication>
#include <QDate>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QDebug>



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
    , mTimeOfNextStart(0)
{
    mTimer.setSingleShot(true);

    connect(&mTimer, SIGNAL(timeout()), this, SLOT(timeout()));
}

Rules::~Rules()
{
    Q_ASSERT(!isRunning());
}

void Rules::start()
{
    Q_ASSERT(!isRunning());

    mNeedRestart = false;

    mExecutorThread = new ExecutorThread(this, this);

    connect(mExecutorThread, SIGNAL(progressChanged(quint8)), this, SLOT(progressChanged(quint8)));
    connect(mExecutorThread, SIGNAL(finished()),              this, SLOT(executorThreadFinished()));

    mExecutorThread->start(QThread::LowestPriority);

    qDebug() << "Executor started";
    emit started();

    mTimeOfNextStart = calculateTimeOfNextStart();

    if (mTimeOfNextStart != 0)
    {
        save();

        mTimer.start(mTimeOfNextStart - QDateTime::currentMSecsSinceEpoch());
        qDebug() << "Next start scheduled on" << QDateTime::fromMSecsSinceEpoch(mTimeOfNextStart).toString("yyyy-MM-dd hh:mm:ss.zzz").toLatin1().constData();
    }
}

void Rules::stop()
{
    qDebug() << "Executor stopped";

    Q_ASSERT(isRunning());

    mNeedRestart = false;

    mExecutorThread->blockSignals(true);

    mExecutorThread->stop();
}

void Rules::waitForFinished()
{
    Q_ASSERT(isRunning());

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
            qint64 delay = mTimeOfNextStart - QDateTime::currentMSecsSinceEpoch();

            if (mTimeOfNextStart == 0 || delay < 0)
            {
                start();
            }
            else
            {
                mTimer.start(delay);
                qDebug() << "Next start scheduled on" << QDateTime::fromMSecsSinceEpoch(mTimeOfNextStart).toString("yyyy-MM-dd hh:mm:ss.zzz").toLatin1().constData();
            }
        }
        break;

        case SCHEDULE_TYPE_DAYS:
        {
            qint64 delay = mTimeOfNextStart - QDateTime::currentMSecsSinceEpoch();

            if (mTimeOfNextStart == 0 || delay < 0)
            {
                if (mDays & (1 << (QDate::currentDate().dayOfWeek() - 1)))
                {
                    start();
                }
                else
                {
                    mTimeOfNextStart = calculateTimeOfNextStart();
                    save();

                    mTimer.start(mTimeOfNextStart - QDateTime::currentMSecsSinceEpoch());
                    qDebug() << "Next start scheduled on" << QDateTime::fromMSecsSinceEpoch(mTimeOfNextStart).toString("yyyy-MM-dd hh:mm:ss.zzz").toLatin1().constData();
                }
            }
            else
            {
                mTimer.start(delay);
                qDebug() << "Next start scheduled on" << QDateTime::fromMSecsSinceEpoch(mTimeOfNextStart).toString("yyyy-MM-dd hh:mm:ss.zzz").toLatin1().constData();
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

    mTimer.stop();
    mTimeOfNextStart  = 0;
}

void Rules::save()
{
    Q_ASSERT(mName != "");

    QString dir = QApplication::applicationDirPath();
    QDir appDir(dir);
    appDir.mkdir(mName);

    QSettings settings(dir + "/" + mName + "/config.ini", QSettings::IniFormat);

    settings.beginGroup("Properties");
    settings.setValue("Type",             toString(mType));
    settings.setValue("EachMinutes",      mEachMinutes);
    settings.setValue("Days",             mDays);
    settings.setValue("CheckAutorun",     mCheckAutorun);
    settings.setValue("CheckSystemFiles", mCheckSystemFiles);
    settings.endGroup();

    settings.beginGroup("Dynamic");
    settings.setValue("TimeOfNextStart", mTimeOfNextStart);
    settings.endGroup();
}

void Rules::load()
{
    Q_ASSERT(mName != "");

    QString dir = QApplication::applicationDirPath();

    Q_ASSERT(QFile::exists(dir + "/" + mName + "/config.ini"));
    QSettings settings(dir + "/" + mName + "/config.ini", QSettings::IniFormat);

    settings.beginGroup("Properties");
    mType             = scheduleTypeFromString(settings.value("Type", "Each minutes").toString());
    mEachMinutes      = settings.value("EachMinutes",      1).toUInt();
    mDays             = settings.value("Days",             SCHEDULE_DAYS_ALL).toUInt() & SCHEDULE_DAYS_MASK;
    mCheckAutorun     = settings.value("CheckAutorun",     true).toBool();
    mCheckSystemFiles = settings.value("CheckSystemFiles", true).toBool();
    settings.endGroup();

    settings.beginGroup("Dynamic");
    mTimeOfNextStart = settings.value("TimeOfNextStart", 0).toLongLong();
    settings.endGroup();
}

void Rules::deleteFolder()
{
    Q_ASSERT(mName != "");
    Q_ASSERT(!isRunning());

    QDir rulesDir(QApplication::applicationDirPath() + "/" + mName);
    rulesDir.removeRecursively();
}

qint64 Rules::calculateTimeOfNextStart()
{
    switch (mType)
    {
        case SCHEDULE_TYPE_MANUALLY:
        {
            return 0;
        }
        break;

        case SCHEDULE_TYPE_EACH_MINUTES:
        {
            return QDateTime::currentMSecsSinceEpoch() + 60000 * mEachMinutes;
        }
        break;

        case SCHEDULE_TYPE_DAYS:
        {
            quint8 days = 0;

            do
            {
                ++days;
            } while ((mDays & (1 << ((QDate::currentDate().dayOfWeek() - 1 + days) % 7))) == 0);

            Q_ASSERT(days <= 7);

            return QDateTime::currentMSecsSinceEpoch() + 86400000 * days;
        }
        break;

        default:
        {
            qFatal("Unknown schedule type");
        }
        break;
    }

    return 0;
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

const QString& Rules::getName() const
{
    return mName;
}

void Rules::setName(const QString &name)
{
    mName = name;
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

void Rules::setEachMinutes(quint16 value)
{
    if (value < 1)
    {
        value = 1;
    }

    mEachMinutes = value;
}

quint8 Rules::getDaysMask() const
{
    return mDays;
}

void Rules::setDaysMask(quint8 value)
{
    mDays = value & SCHEDULE_DAYS_MASK;
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

void Rules::timeout()
{
    if (isRunning())
    {
        qDebug() << "Rules starting delayed";

        mNeedRestart = true;
    }
    else
    {
        start();
    }
}

void Rules::progressChanged(quint8 progress)
{
    if (isRunning())
    {
        Q_ASSERT(mProgressBar);

        mProgressBar->setValue(progress);
    }
}

void Rules::executorThreadFinished()
{
    mProgressBar->setValue(0);

    qDebug() << "Executor finished";
    emit finished(mExecutorThread->isReportCreated());

    delete mExecutorThread;
    mExecutorThread = 0;

    if (mNeedRestart)
    {
        start();
    }
}
