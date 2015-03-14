#include "rules.h"



Rules::Rules()
    : mType(SCHEDULER_TYPE_EACH_MINUTES)
    , mEachMinutes(1)
    , mDays(0xFF)
    , mCheckAutorun(true)
    , mCheckSystemFiles(true)
{

}

Rules::~Rules()
{

}

const QString Rules::toString() const
{
    switch (mType)
    {
        case SCHEDULER_TYPE_MANUALLY:       return tr("Manually");
        case SCHEDULER_TYPE_EACH_MINUTES:   return tr("Each %1 minute(s)").arg(mEachMinutes);
        case SCHEDULER_TYPE_DAYS:
        {
            QString res;

            if (mDays & SCHEDULER_DAYS_MONDAY)
            {
                if (res != "")
                {
                    res.append(";");
                }

                res.append(tr("Mon"));
            }

            if (mDays & SCHEDULER_DAYS_TUESDAY)
            {
                if (res != "")
                {
                    res.append(";");
                }

                res.append(tr("Tue"));
            }

            if (mDays & SCHEDULER_DAYS_WEDNESDAY)
            {
                if (res != "")
                {
                    res.append(";");
                }

                res.append(tr("Wed"));
            }

            if (mDays & SCHEDULER_DAYS_THURSDAY)
            {
                if (res != "")
                {
                    res.append(";");
                }

                res.append(tr("Thu"));
            }

            if (mDays & SCHEDULER_DAYS_FRIDAY)
            {
                if (res != "")
                {
                    res.append(";");
                }

                res.append(tr("Fri"));
            }

            if (mDays & SCHEDULER_DAYS_SATURDAY)
            {
                if (res != "")
                {
                    res.append(";");
                }

                res.append(tr("Sat"));
            }

            if (mDays & SCHEDULER_DAYS_SUNDAY)
            {
                if (res != "")
                {
                    res.append(";");
                }

                res.append(tr("Sun"));
            }

            return res;
        }

        default: return "UNKNOWN SCHEDULER TYPE";
    }
}

Rules::SchedulerType Rules::getType() const
{
    return mType;
}

void Rules::setType(Rules::SchedulerType value)
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

bool Rules::isMondayEnabled() const
{
    return mDays & SCHEDULER_DAYS_MONDAY;
}

void Rules::setMondayEnabled(bool value)
{
    if (value)
    {
        mDays |= SCHEDULER_DAYS_MONDAY;
    }
    else
    {
        mDays &= ~SCHEDULER_DAYS_MONDAY;
    }
}

bool Rules::isTuesdayEnabled() const
{
    return mDays & SCHEDULER_DAYS_TUESDAY;
}

void Rules::setTuesdayEnabled(bool value)
{
    if (value)
    {
        mDays |= SCHEDULER_DAYS_TUESDAY;
    }
    else
    {
        mDays &= ~SCHEDULER_DAYS_TUESDAY;
    }
}

bool Rules::isWednesdayEnabled() const
{
    return mDays & SCHEDULER_DAYS_WEDNESDAY;
}

void Rules::setWednesdayEnabled(bool value)
{
    if (value)
    {
        mDays |= SCHEDULER_DAYS_WEDNESDAY;
    }
    else
    {
        mDays &= ~SCHEDULER_DAYS_WEDNESDAY;
    }
}

bool Rules::isThursdayEnabled() const
{
    return mDays & SCHEDULER_DAYS_THURSDAY;
}

void Rules::setThursdayEnabled(bool value)
{
    if (value)
    {
        mDays |= SCHEDULER_DAYS_THURSDAY;
    }
    else
    {
        mDays &= ~SCHEDULER_DAYS_THURSDAY;
    }
}

bool Rules::isFridayEnabled() const
{
    return mDays & SCHEDULER_DAYS_FRIDAY;
}

void Rules::setFridayEnabled(bool value)
{
    if (value)
    {
        mDays |= SCHEDULER_DAYS_FRIDAY;
    }
    else
    {
        mDays &= ~SCHEDULER_DAYS_FRIDAY;
    }
}

bool Rules::isSaturdayEnabled() const
{
    return mDays & SCHEDULER_DAYS_SATURDAY;
}

void Rules::setSaturdayEnabled(bool value)
{
    if (value)
    {
        mDays |= SCHEDULER_DAYS_SATURDAY;
    }
    else
    {
        mDays &= ~SCHEDULER_DAYS_SATURDAY;
    }
}

bool Rules::isSundayEnabled() const
{
    return mDays & SCHEDULER_DAYS_SUNDAY;
}

void Rules::setSundayEnabled(bool value)
{
    if (value)
    {
        mDays |= SCHEDULER_DAYS_SUNDAY;
    }
    else
    {
        mDays &= ~SCHEDULER_DAYS_SUNDAY;
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
