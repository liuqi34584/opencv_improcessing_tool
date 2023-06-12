#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowIcon(QIcon(QIcon(":/icons/opencv.ico")));
    w.setFixedSize(1450, 800);
    w.show();
    return a.exec();
}
