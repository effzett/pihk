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
        mainwindow.cpp \
        preferences.cpp

HEADERS  += mainwindow.h \
    about.h \
    preferences.h

FORMS    += mainwindow.ui \
    about.ui \
    preferences.ui

ICON = pihk2.icns
