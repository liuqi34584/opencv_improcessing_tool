#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowIcon(QIcon(QIcon(":/icons/icon.png")));
    w.setFixedSize(860, 880);
    w.show();
    return a.exec();
}
