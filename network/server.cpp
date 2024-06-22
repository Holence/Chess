#include "server.h"

Server::Server(QObject *parent) : Peer{parent} {
    isServer = true;
    tcpServer = new QTcpServer(parent);
    tcpServer->listen(QHostAddress::Any, 11451);
    connect(tcpServer, &QTcpServer::newConnection, this, &Server::connectToPeer);

    // server会每隔5秒向client发时间戳，client需要回复，这样双方能计算通信的delay
    timer = new QTimer(this);
    timer->setInterval(5 * 1000);
    connect(timer, &QTimer::timeout, this, &Peer::sendTime);
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
