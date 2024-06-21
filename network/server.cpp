#include "server.h"

Server::Server(QObject *parent) : Peer{parent} {
    isServer = true;
    tcpServer = new QTcpServer(parent);
    tcpServer->listen(QHostAddress::Any, 11451);
    connect(tcpServer, &QTcpServer::newConnection, this, &Server::connectToPeer);
}

void Server::connectToPeer() {
    // 只允许一个Client连接
    if (!tcpSocket) {
        tcpSocket = tcpServer->nextPendingConnection();
        bondSocketSignalSlot();

        // 给client发送颜色
        sendInializePack();
    }
}
