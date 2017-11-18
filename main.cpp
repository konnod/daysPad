#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //open file and read map from it
    if(!w.readMapFromFile())
        return 0;
    //remove map's records, if they are out of date
    w.cleanUpMap();
    w.start();
    w.show();
    return a.exec();
}
