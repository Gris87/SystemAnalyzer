#-------------------------------------------------
#
# Project created by QtCreator 2015-02-09T23:09:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET   = SystemAnalyzer
TEMPLATE = app

RC_FILE    = Resources.rc
RESOURCES += Resources.qrc

SOURCES += \
            src/main/mainwindow.cpp \
            src/main/trayicon.cpp \
            src/main.cpp \

HEADERS  += src/main/mainwindow.h \
            src/main/trayicon.h

FORMS    += src/main/mainwindow.ui
