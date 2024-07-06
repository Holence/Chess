#include "server.h"

Server::Server(QObject *parent) : Peer{parent} {
    isServer = true;
    tcpServer = new QTcpServer(parent);
    // Server默认端口
#ifndef RTS_MODE
    if (!tcpServer->listen(QHostAddress::Any, 11451)) {
#else
    if (!tcpServer->listen(QHostAddress::Any, 11452)) {
#endif
        // 被占用的话，再随机分配
        tcpServer->listen(QHostAddress::Any, 0);
    }
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
