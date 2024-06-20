#include "peer.h"

Peer::Peer(QObject *parent)
    : QObject{parent} {}

void Peer::handleDataIn() {
    QString data = QString::fromUtf8(tcpSocket->readAll());
    qDebug() << data;
}

void Peer::handelDataOut(QString s) {
    tcpSocket->write(s.toUtf8());
}
