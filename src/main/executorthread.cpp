#include "executorthread.h"

#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QProcessEnvironment>
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



struct SystemFile
{
    QString path;
    QString fileName;

    SystemFile(const QString &_path, const QString &_fileName)
        : path(_path)
        , fileName(_fileName)
    {
    }
};

QList<SystemFile> systemFiles;



ExecutorThread::ExecutorThread(Rules *rules, QObject *parent)
    : QThread(parent)
    , mTerminated(false)
    , mRules(rules)
    , mStepsCount(0)
    , mBaseProgress(0)
    , mProgress(0)
    , mReportCreated(false)
{
    if (systemFiles.length() == 0)
    {
#ifdef Q_OS_WIN
        {
            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

            QString windowsDir = QDir::fromNativeSeparators(env.value("WINDIR"));

            if (windowsDir != "")
            {
                systemFiles.append(SystemFile(windowsDir + "/System32/drivers/etc/hosts", "hosts.txt"));
            }
            else
            {
                qFatal("There is no WINDIR environment key");
            }
        }
#else
#error "There is no system files to check"
#endif
    }
}

ExecutorThread::~ExecutorThread()
{
}

void ExecutorThread::stop()
{
    mTerminated = true;
}

bool ExecutorThread::isReportCreated()
{
    return mReportCreated;
}

void ExecutorThread::run()
{
    mTimestamp = QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss_zzz");

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
        Q_ASSERT(progress > mProgress && progress <= 100);
        mProgress = progress;

        emit progressChanged(mProgress);
    }
}

void ExecutorThread::compareAndStore(const QByteArray &cachedArray, const QByteArray &newArray, const QString &category, const QString &fileName)
{
    bool same = true;

    if (cachedArray.length() != newArray.length())
    {
        same = false;
    }
    else
    {
        int sizeOfLong  = sizeof(long);
        int tail        = cachedArray.length() % sizeOfLong;
        int blocksCount = (cachedArray.length() - tail) / sizeOfLong;

        const char *cachedBytes = cachedArray.constData();
        const char *newBytes    = newArray.constData();
        const long *cachedLongs = reinterpret_cast<const long *>(cachedBytes);
        const long *newLongs    = reinterpret_cast<const long *>(newBytes);

        for (int i = 0; i < blocksCount && !mTerminated; ++i)
        {
            if (cachedLongs[i] != newLongs[i])
            {
                same = false;
                break;
            }
        }

        if (mTerminated)
        {
            return;
        }

        if (same)
        {
            for (int i = 0; i < tail && !mTerminated; ++i)
            {
                if (cachedBytes[blocksCount * sizeOfLong + i] != newBytes[blocksCount * sizeOfLong + i])
                {
                    same = false;
                    break;
                }
            }

            if (mTerminated)
            {
                return;
            }
        }
    }

    if (!same)
    {
        // DO NOT TERMINATE THREAD IN THIS BLOCK
        QDir tempDir;

        QString cacheDir = QApplication::applicationDirPath() + "/" + mRules->getName() + "/Cache/" + category;
        QFile cachedFile(cacheDir + "/" + fileName);

        if (cachedFile.exists())
        {
            mReportCreated = true;

            QString reportDir = QApplication::applicationDirPath() + "/" + mRules->getName() + "/Reports/" + mTimestamp + "/" + category;
            tempDir.mkpath(reportDir + "/1_before");
            tempDir.mkpath(reportDir + "/2_after");



            QFile beforeFile(reportDir + "/1_before/" + fileName);

            beforeFile.open(QIODevice::WriteOnly);
            cachedFile.open(QIODevice::ReadOnly);

            while (!cachedFile.atEnd())
            {
                beforeFile.write(cachedFile.read(4096));
            }

            cachedFile.close();
            beforeFile.close();



            QFile afterFile(reportDir + "/2_after/" + fileName);

            afterFile.open(QIODevice::WriteOnly);
            afterFile.write(newArray);
            afterFile.close();
        }

        tempDir.mkpath(cacheDir);

        cachedFile.open(QIODevice::WriteOnly);
        cachedFile.write(newArray);
        cachedFile.close();
    }
}

void ExecutorThread::checkAutorun()
{
    STEP_INITIALIZATION();

    quint8 step = 0;

    // Windows
#ifdef Q_OS_WIN
    {
        quint8 stepsCount = 8;

        CHECK_AUTORUN_STEP(checkAutorun_HKCU_Run,        step, stepsCount);
        CHECK_AUTORUN_STEP(checkAutorun_HKCU_RunOnce,    step, stepsCount);
        CHECK_AUTORUN_STEP(checkAutorun_HKLM_Run,        step, stepsCount);
        CHECK_AUTORUN_STEP(checkAutorun_HKLM_RunOnce,    step, stepsCount);
        CHECK_AUTORUN_STEP(checkAutorun_StartUp_User1,   step, stepsCount);
        CHECK_AUTORUN_STEP(checkAutorun_StartUp_User2,   step, stepsCount);
        CHECK_AUTORUN_STEP(checkAutorun_StartUp_Common1, step, stepsCount);
        CHECK_AUTORUN_STEP(checkAutorun_StartUp_Common2, step, stepsCount);
    }
#endif
}

#ifdef Q_OS_WIN
void ExecutorThread::checkAutorun_VerifyRegistry(const QString &regKey, const QString &fileName)
{
    QString res = regKey;
    res.append("\n");
    res.append("\n");

    QSettings settings(regKey, QSettings::NativeFormat);
    QStringList keys = settings.allKeys();

    for (int i = 0; i < keys.length() && !mTerminated; ++i)
    {
        const QString &key = keys.at(i);

        // Ignore this key since it always changes on reboot
        if (key != "Adobe Speed Launcher")
        {
            QString value = settings.value(key).toString();

            res.append(key);
            res.append(" = ");
            res.append(value);
            res.append("\n");
        }
    }

    if (mTerminated)
    {
        return;
    }

    QByteArray cachedArray;

    QFile cachedFile(QApplication::applicationDirPath() + "/" + mRules->getName() + "/Cache/Autorun/" + fileName);

    if (cachedFile.exists())
    {
        cachedFile.open(QIODevice::ReadOnly);

        while (!cachedFile.atEnd() && !mTerminated)
        {
            cachedArray.append(cachedFile.read(4096));
        }

        cachedFile.close();
    }

    if (mTerminated)
    {
        return;
    }

    compareAndStore(cachedArray, res.toLocal8Bit(), "Autorun", fileName);
}

void ExecutorThread::checkAutorun_HKCU_Run()
{
    checkAutorun_VerifyRegistry("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",      "HKCU_Run.txt");
}

void ExecutorThread::checkAutorun_HKCU_RunOnce()
{
    checkAutorun_VerifyRegistry("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce",  "HKCU_RunOnce.txt");
}

void ExecutorThread::checkAutorun_HKLM_Run()
{
    checkAutorun_VerifyRegistry("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",     "HKLM_Run.txt");
}

void ExecutorThread::checkAutorun_HKLM_RunOnce()
{
    checkAutorun_VerifyRegistry("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce", "HKLM_RunOnce.txt");
}

void ExecutorThread::checkAutorun_StartUpVerifyRegistry(const QString &regKey, const QString &key, const QString &fileName)
{
    QSettings settings(regKey, QSettings::NativeFormat);

    QString startupPath = settings.value(key).toString();

    if (startupPath != "")
    {
        QString res = startupPath;
        res.append("\n");
        res.append("\n");

        QDir startupDir(startupPath);
        QFileInfoList files = startupDir.entryInfoList(
                                                       QDir::AllEntries | QDir::NoDotAndDotDot | QDir::System | QDir::Hidden,
                                                       QDir::Name | QDir::DirsFirst | QDir::IgnoreCase
                                                      );

        for (int i = 0; i < files.length() && !mTerminated; ++i)
        {
            const QFileInfo &fileInfo = files.at(i);

            res.append(fileInfo.fileName());
            res.append("    ");

            if (fileInfo.isDir())
            {
                res.append("DIR");
            }
            else
            {
                res.append("    ");
                res.append(fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss.zzz"));
                res.append("    ");

                if (fileInfo.isSymLink())
                {
                    res.append("LINK => ");
                    res.append(fileInfo.symLinkTarget());
                }
                else
                {
                    res.append("FILE");
                    res.append("    ");
                    res.append(QString::number(QFile(fileInfo.absoluteFilePath()).size()));
                }
            }

            res.append("\n");
        }

        if (mTerminated)
        {
            return;
        }

        QByteArray cachedArray;

        QFile cachedFile(QApplication::applicationDirPath() + "/" + mRules->getName() + "/Cache/Autorun/" + fileName);

        if (cachedFile.exists())
        {
            cachedFile.open(QIODevice::ReadOnly);

            while (!cachedFile.atEnd() && !mTerminated)
            {
                cachedArray.append(cachedFile.read(4096));
            }

            cachedFile.close();
        }

        if (mTerminated)
        {
            return;
        }

        compareAndStore(cachedArray, res.toLocal8Bit(), "Autorun", fileName);
    }
}

void ExecutorThread::checkAutorun_StartUp_User1()
{
    checkAutorun_StartUpVerifyRegistry("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",       "Startup",        "StartUp_User1.txt");
}

void ExecutorThread::checkAutorun_StartUp_User2()
{
    checkAutorun_StartUpVerifyRegistry("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders",  "Startup",        "StartUp_User2.txt");
}

void ExecutorThread::checkAutorun_StartUp_Common1()
{
    checkAutorun_StartUpVerifyRegistry("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",      "Common Startup", "StartUp_Common1.txt");
}

void ExecutorThread::checkAutorun_StartUp_Common2()
{
    checkAutorun_StartUpVerifyRegistry("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders", "Common Startup", "StartUp_Common2.txt");
}
#endif

void ExecutorThread::checkSystemFiles()
{
    STEP_INITIALIZATION();

    for (int i = 0; i < systemFiles.length() && !mTerminated; ++i)
    {
        const SystemFile &systemFile = systemFiles.at(i);

        QByteArray newArray;

        QFile newFile(systemFile.path);

        if (newFile.exists())
        {
            newFile.open(QIODevice::ReadOnly);

            while (!newFile.atEnd() && !mTerminated)
            {
                newArray.append(newFile.read(4096));
            }

            newFile.close();
        }

        if (mTerminated)
        {
            return;
        }

        QByteArray cachedArray;

        QFile cachedFile(QApplication::applicationDirPath() + "/" + mRules->getName() + "/Cache/SystemFiles/" + systemFile.fileName);

        if (cachedFile.exists())
        {
            cachedFile.open(QIODevice::ReadOnly);

            while (!cachedFile.atEnd() && !mTerminated)
            {
                cachedArray.append(cachedFile.read(4096));
            }

            cachedFile.close();
        }

        if (mTerminated)
        {
            return;
        }

        compareAndStore(cachedArray, newArray, "SystemFiles", systemFile.fileName);

        updateProgress((i + 1) * 100 / systemFiles.length());
    }
}
