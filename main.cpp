#include "mainwindow.h"
#include "db.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!DB::instance().connect()) {
        return -1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}