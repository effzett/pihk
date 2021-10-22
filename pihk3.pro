#-------------------------------------------------
#
# Project created by QtCreator 2016-06-16T22:36:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PIHK
TEMPLATE = app
CONFIG += app_bundle

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

#ifdef Q_OS_DARWIN
FORMS    += mainwindow.ui \
    about.ui \
    lizenz.ui \
    preferences.ui \
    regularien.ui
#elif defined(Q_OS_WIN)
FORMS    += mainwindowwin.ui \
    about.ui \
    lizenz.ui \
    preferences.ui \
    regularien.ui
#else
#error "Is not supported!" 
#endif


#ifdef Q_OS_DARWIN
ICON = myLogoPIHK3a.icns
#elif defined(Q_OS_WIN)
RC_ICONS = myLogoPIHK3a.ico
#else
#error "Is not supported!" 
#endif


RESOURCES += \
    resourcen.qrc
    

DISTFILES +=
