#include "mainwindow.h"

#include <QApplication>
#include "header.h"

int main(int argc, char *argv[])
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
