#include "mainwindow.h"

#include <QApplication>

QT_USE_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.setGeometry(350,100,850,750);
    w.show();

    return a.exec();
}
