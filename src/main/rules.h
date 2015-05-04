#ifndef RULES_H
#define RULES_H

#include <QObject>

#include <QProgressBar>

#include "executorthread.h"



class Rules : QObject
{
    Q_OBJECT

public:
    enum ScheduleType
    {
          SCHEDULE_TYPE_MANUALLY
        , SCHEDULE_TYPE_EACH_MINUTES
        , SCHEDULE_TYPE_DAYS
    };

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
    void checkIfNeedStart();
    void reset();

    const QString toString() const;

    ScheduleType getType() const;
    void         setType(ScheduleType value);

    quint16 getEachMinutes() const;
    void    setEachMinutes(quint16 value);

    quint8 getDaysMask() const;

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

private slots:
    void progressChanged(int progress);
    void executorThreadFinished();

private:
    ScheduleType    mType;
    quint16         mEachMinutes;
    quint8          mDays;
    bool            mCheckAutorun;
    bool            mCheckSystemFiles;

    QProgressBar   *mProgressBar;
    ExecutorThread *mExecutorThread;
    bool            mNeedRestart;
};

#endif // RULES_H
