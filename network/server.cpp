#include "server.h"

Server::Server(QObject *parent) : Peer{parent} {
    tcpServer = new QTcpServer(parent);
    tcpServer->listen(QHostAddress::Any, 11451);
    connect(tcpServer, &QTcpServer::newConnection, this, &Server::connectToPeer);
}

int Server::getPort() {
    return tcpServer->serverPort();
}

void Server::connectToPeer() {
    // 只允许一个Client连接
    if (!tcpSocket) {
        tcpSocket = tcpServer->nextPendingConnection();

        connect(tcpSocket, &QAbstractSocket::errorOccurred, this, [this] {
            emit socketError(tcpSocket->errorString());
        });
        connect(tcpSocket, &QTcpSocket::readyRead, this, &Server::handleDataIn);
        connect(tcpSocket, &QTcpSocket::connected, this, &Server::connectSuccessed);

        emit connectSuccessed();
    }
}
