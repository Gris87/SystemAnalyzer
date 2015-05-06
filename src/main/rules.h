#ifndef RULES_H
#define RULES_H

#include <QObject>

#include <QProgressBar>
#include <QTimer>

#include "executorthread.h"



class Rules : public QObject
{
    Q_OBJECT

public:
    enum ScheduleType
    {
          SCHEDULE_TYPE_MANUALLY
        , SCHEDULE_TYPE_EACH_MINUTES
        , SCHEDULE_TYPE_DAYS
    };

    static const QString toString(ScheduleType type)
    {
        switch (type)
        {
            case SCHEDULE_TYPE_MANUALLY:     return "Manually";
            case SCHEDULE_TYPE_EACH_MINUTES: return "Each minutes";
            case SCHEDULE_TYPE_DAYS:         return "Days";
            default:                         return "UNKNOWN";
        }
    }

    static ScheduleType scheduleTypeFromString(const QString &type)
    {
        if (type == "Manually")
        {
            return SCHEDULE_TYPE_MANUALLY;
        }

        if (type == "Each minutes")
        {
            return SCHEDULE_TYPE_EACH_MINUTES;
        }

        if (type == "Days")
        {
            return SCHEDULE_TYPE_DAYS;
        }

        qFatal("Unknown schedule type");

        return SCHEDULE_TYPE_MANUALLY;
    }

    enum ScheduleDays
    {
          SCHEDULE_DAYS_MONDAY    = 0x01
        , SCHEDULE_DAYS_TUESDAY   = 0x02
        , SCHEDULE_DAYS_WEDNESDAY = 0x04
        , SCHEDULE_DAYS_THURSDAY  = 0x08
        , SCHEDULE_DAYS_FRIDAY    = 0x10
        , SCHEDULE_DAYS_SATURDAY  = 0x20
        , SCHEDULE_DAYS_SUNDAY    = 0x40
        , SCHEDULE_DAYS_ALL       = SCHEDULE_DAYS_MONDAY   | SCHEDULE_DAYS_TUESDAY | SCHEDULE_DAYS_WEDNESDAY |
                                    SCHEDULE_DAYS_THURSDAY | SCHEDULE_DAYS_FRIDAY  | SCHEDULE_DAYS_SATURDAY  |
                                    SCHEDULE_DAYS_SUNDAY
        , SCHEDULE_DAYS_MASK      = SCHEDULE_DAYS_ALL
    };

    Rules(QObject *parent = 0);
    ~Rules();

    void start();
    void stop();
    void waitForFinished();
    void checkIfNeedStart();
    void reset();

    void save();
    void load();
    void deleteFolder();

    qint64 calculateTimeOfNextStart();

    const QString toString() const;

    const QString& getName() const;
    void           setName(const QString &name);

    ScheduleType getType() const;
    void         setType(ScheduleType value);

    quint16 getEachMinutes() const;
    void    setEachMinutes(quint16 value);

    quint8 getDaysMask() const;
    void setDaysMask(quint8 value);

    bool isMondayEnabled() const;
    void setMondayEnabled(bool value);

    bool isTuesdayEnabled() const;
    void setTuesdayEnabled(bool value);

    bool isWednesdayEnabled() const;
    void setWednesdayEnabled(bool value);

    bool isThursdayEnabled() const;
    void setThursdayEnabled(bool value);

    bool isFridayEnabled() const;
    void setFridayEnabled(bool value);

    bool isSaturdayEnabled() const;
    void setSaturdayEnabled(bool value);

    bool isSundayEnabled() const;
    void setSundayEnabled(bool value);

    bool isCheckAutorun() const;
    void setCheckAutorun(bool value);

    bool isCheckSystemFiles() const;
    void setCheckSystemFiles(bool value);

    void setProgressBar(QProgressBar *progressBar);

    bool isRunning() const;

signals:
    void started();
    void finished();

private slots:
    void timeout();
    void progressChanged(quint8 progress);
    void executorThreadFinished();

private:
    QString         mName;
    ScheduleType    mType;
    quint16         mEachMinutes;
    quint8          mDays;
    bool            mCheckAutorun;
    bool            mCheckSystemFiles;

    QProgressBar   *mProgressBar;
    ExecutorThread *mExecutorThread;
    bool            mNeedRestart;
    QTimer          mTimer;
    qint64          mTimeOfNextStart;
};

#endif // RULES_H
