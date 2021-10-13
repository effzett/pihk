#-------------------------------------------------
#
# Project created by QtCreator 2016-06-16T22:36:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PIHK
TEMPLATE = app


SOURCES += main.cpp\
        about.cpp \
        lizenz.cpp \
        mainwindow.cpp \
        preferences.cpp \
        regularien.cpp

HEADERS  += mainwindow.h \
    about.h \
    lizenz.h \
    preferences.h \
    regularien.h

FORMS    += mainwindow.ui \
    about.ui \
    lizenz.ui \
    preferences.ui \
    regularien.ui

ICON = pihk2.icns

RESOURCES += \
    resourcen.qrc

DISTFILES +=
