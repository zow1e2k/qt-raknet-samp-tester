#include "mainwindow.h"

#include <QApplication>
#include "RakNet/src/RakPeerInterface.h"
#include <stdio.h>

#define MAX_CLIENTS 10
#define SERVER_PORT 60000

int main(int argc, char *argv[]) {
    RakNet::RakPeerInterface *peer = RakNet::RakPeerInterface::GetInstance();

    printf("(C) or (S)erver?\n");
    RakNet::SocketDescriptor sd;
    peer->Startup(1,&sd, 1);

    RakNet::RakPeerInterface::DestroyInstance(peer);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
