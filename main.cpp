#include "mainwindow.h"

#include <QApplication>

#define MAX_CLIENTS 10
#define SERVER_PORT 60000

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
