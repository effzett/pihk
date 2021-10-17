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
        prefs.cpp \
        regularien.cpp \
        treeitem.cpp \
        treemodel.cpp

HEADERS  += mainwindow.h \
    about.h \
    lizenz.h \
    mypihk.h \
    preferences.h \
    prefs.h \
    regularien.h \
    treeitem.h \
    treemodel.h

FORMS    += mainwindow.ui \
    about.ui \
    lizenz.ui \
    preferences.ui \
    regularien.ui

ICON = pihk2.icns

RESOURCES += \
    resourcen.qrc

DISTFILES +=
