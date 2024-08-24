#include "mainwindow.h"
#define RAKNET_BUILD_FOR_CLIENT

#include <QApplication>
#include <QThread>

#include "RakNet/includes/RakNetworkFactory.h"
#include "RakNet/includes/RakClientInterface.h"
#include "RakNet/includes/PacketEnumerations.h"

using namespace RakNet;

void connect(RakClientInterface *peer);

enum {
    CLIENT_STATE_CONNECTING,
    CLIENT_STATE_CONNECTED,
    CLIENT_STATE_WAITING
};

unsigned int playerState = CLIENT_STATE_WAITING;

void UpdateNetwork(RakClientInterface *pRakNet) {
    unsigned char packetIdentifier;
    Packet *pkt;

    while((pkt = pRakNet->Receive())) {
        if ( ( unsigned char ) pkt->data[ 0 ] == ID_TIMESTAMP ) {

            if ( pkt->length > sizeof( unsigned char ) + sizeof( unsigned int ) )
                packetIdentifier = ( unsigned char ) pkt->data[ sizeof( unsigned char ) + sizeof( unsigned int ) ];
            else {
                qDebug() << "pkt->length < size: unsigned char + unsigned int";
                return;
            }
        } else {
            packetIdentifier = (unsigned char)pkt->data[0];
        }

        switch(packetIdentifier) {

            case ID_DISCONNECTION_NOTIFICATION:
                qDebug() << " Connection was closed by the server. Reconnecting in 5 seconds.";
                QThread::msleep(5000);
                connect(pRakNet);
                break;
            case ID_CONNECTION_BANNED:
                qDebug() << "You are banned. Reconnecting in 5 seconds.";
                QThread::msleep(5000);
                connect(pRakNet);
                break;
            case ID_CONNECTION_ATTEMPT_FAILED:
                qDebug() << "Connection attempt failed. Reconnecting in 5 seconds.";
                qDebug() << "packetIdentifier = " << packetIdentifier << " " << pkt->data[1] << " " << pkt->data[2] << " " << pkt->data[3];
                //QThread::msleep(5000);
                connect(pRakNet);
                break;
            case ID_NO_FREE_INCOMING_CONNECTIONS:
                qDebug() << "The server is full. Reconnecting in 5 seconds.";
                QThread::msleep(5000);
                connect(pRakNet);
                break;
            case ID_INVALID_PASSWORD:
                qDebug() << "Invalid password. Reconnecting in 5 seconds.";
                connect(pRakNet);
                break;
            case ID_CONNECTION_LOST:
                qDebug() << "The connection was lost.";
                break;
            case ID_CONNECTION_REQUEST_ACCEPTED:
                qDebug() << "The connection accepted.";
                break;
            case ID_AUTH_KEY:
                qDebug() << "Packet_AUTH_KEY.";
                break;

            default:
                qDebug() << "packetIdentifier = " << packetIdentifier << " " << pkt->data[1] << " " << pkt->data[2] << " " << pkt->data[3];
                break;
        }

        pRakNet->DeallocatePacket(pkt);
    }
}

void connect(RakClientInterface *pRakNet) {
    pRakNet->Connect("46.174.55.171", 7777, 0, 0, 5);
}

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QThread *RakNetThread = QThread::create([] {
        RakClientInterface* pRakClient = RakNetworkFactory::GetRakClientInterface();

        if (pRakClient == nullptr) {
            qDebug() << "pRakClient is nullptr";
            RakNetworkFactory::DestroyRakClientInterface(pRakClient);
            return;
        }

        pRakClient->SetMTUSize(576); // default SA:MP MTU size (server.cfg)
        pRakClient->SetPassword("");

        connect(pRakClient);
        qDebug() << "RakNetThread started";

        while (true) {
            if (playerState == CLIENT_STATE_WAITING) {
                UpdateNetwork(pRakClient);
            } else if (playerState == CLIENT_STATE_CONNECTED) {
                qDebug() << "successfully conected";
                break;
            }

            QThread::msleep(200);
        }

        pRakClient->Disconnect(0);
        RakNetworkFactory::DestroyRakClientInterface(pRakClient);
    });
    RakNetThread->start();

    return a.exec();
}
