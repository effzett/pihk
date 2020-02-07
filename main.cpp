#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::RoundPreferFloor);

#endif
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
