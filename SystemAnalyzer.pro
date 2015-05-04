#-------------------------------------------------
#
# Project created by QtCreator 2015-02-09T23:09:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET   = SystemAnalyzer
TEMPLATE = app



# Warnings as errors - BEGIN
QMAKE_CFLAGS_WARN_ON   += -Werror
QMAKE_CXXFLAGS_WARN_ON += -Werror
# Warnings as errors - END

# Release optimization - BEGIN
QMAKE_CFLAGS_RELEASE -= -O1
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CFLAGS_RELEASE += -O3

QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
# Release optimization - END



CONFIG += \
            warn_on \
            c++11

RC_FILE    = Resources.rc
RESOURCES += Resources.qrc

SOURCES += \
            src/main/aboutdialog.cpp \
            src/main/editrulesdialog.cpp \
            src/main/executorthread.cpp \
            src/main/mainwindow.cpp \
            src/main/rules.cpp \
            src/main/trayicon.cpp \
            src/main.cpp

HEADERS  += \
            src/main/aboutdialog.h \
            src/main/editrulesdialog.h \
            src/main/executorthread.h \
            src/main/mainwindow.h \
            src/main/rules.h \
            src/main/trayicon.h

FORMS    += \
            src/main/aboutdialog.ui \
            src/main/editrulesdialog.ui \
            src/main/mainwindow.ui

DISTFILES += \
    TODO.txt
