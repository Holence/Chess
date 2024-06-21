#include "client.h"

Client::Client(QObject *parent) : Peer{parent} {
    tcpSocket = new QTcpSocket(parent);

    connect(tcpSocket, &QAbstractSocket::errorOccurred, this, [this] {
        emit socketError(tcpSocket->errorString());
    });
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Client::handleDataIn);
}

int Client::getPort() {
    return tcpSocket->localPort();
}

void Client::connectToServer(QString &hostName, quint16 port) {
    tcpSocket->connectToHost(hostName, port);
}
