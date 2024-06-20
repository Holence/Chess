#include "peer.h"

Peer::Peer(QObject *parent)
    : QObject{parent} {}

void Peer::sendMovement(Movement m) {
    handleDataOut(Movement::toString(m));
}

void Peer::handleDataIn() {
    QString s = QString::fromUtf8(tcpSocket->readAll());
    emit receivedMovement(Movement::fromString(s));
}

void Peer::handleDataOut(QString s) {
    tcpSocket->write(s.toUtf8());
}
