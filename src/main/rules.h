#ifndef RULES_H
#define RULES_H

#include <QObject>



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
    };

    Rules();
    ~Rules();

    void reset();

    const QString toString() const;

    ScheduleType getType() const;
    void         setType(ScheduleType value);

    quint16 getEachMinutes() const;
    void    setEachMinutes(quint16 value);

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

private:
    ScheduleType mType;
    quint16      mEachMinutes;
    quint8       mDays;
    bool         mCheckAutorun;
    bool         mCheckSystemFiles;
};

#endif // RULES_H
