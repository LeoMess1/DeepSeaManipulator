#include "mainwindow.h"

#include <QApplication>

QT_USE_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
//    w.setGeometry(300,100,962,754);
    w.show();

    return a.exec();
}
