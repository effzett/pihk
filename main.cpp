#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QFile>

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    // QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

#endif
    QApplication a(argc, argv);
    
    QTranslator tr1;  
    if(tr1.load(":/qtbase_de")){
        a.installTranslator(&tr1);
    }

    MainWindow w;
    w.show();
    return a.exec();
}
