#include "client.h"

Client::Client(QObject *parent) : Peer{parent} {
    isServer = false;
    tcpSocket = new QTcpSocket(parent);
    bondSocketSignalSlot();
}

void Client::connectToServer(QString &hostName, quint16 port) {
    tcpSocket->connectToHost(hostName, port);
}
