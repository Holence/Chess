#include "peer.h"

Peer::Peer(QObject *parent)
    : QObject{parent} {}

QString Peer::getConnectionInfo() {
    return tcpSocket->localAddress().toString() + ":" + QString::number(tcpSocket->localPort()) + " -> " + tcpSocket->peerAddress().toString() + ":" + QString::number(tcpSocket->peerPort());
}

void Peer::disconnectSocket() {
    if (tcpSocket and tcpSocket->isOpen())
        tcpSocket->close();
}

void Peer::sendMovement(Movement m) {
    handleDataOut("1" + Movement::toString(m));
}

void Peer::sendResign() {
    handleDataOut("9");
}

void Peer::sendMessage(QString s) {
    handleDataOut("2" + s);
}

void Peer::handleDataIn() {
    QString s = QString::fromUtf8(tcpSocket->readAll());
    QString op = s.mid(1);
    switch (s.at(0).digitValue()) {
    case 0:
        if (!isServer) {
            // client收到自己的颜色
            if (op == 'w')
                setSelfColor(Piece_Color::White);
            else
                setSelfColor(Piece_Color::Black);
            // 发回给server进行确认
            sendInializePack();
            emit connectionSuccessed();
        } else {
            // server收到颜色确认
            if (op == selfColor) {
                emit connectionSuccessed();
            } else {
                emit socketError("Color not match!!!");
            }
        }
        break;
    case 1:
        emit receivedMovement(Movement::fromString(op));
        break;
    case 2:
        emit receivedMessage(op);
        break;
    case 9:
        emit receivedResign();
        break;
    default:
        break;
    }
}

void Peer::handleDataOut(QString s) {
    tcpSocket->write(s.toUtf8());
}

void Peer::setSelfColor(Piece_Color color) {
    selfColor = color;
}

void Peer::bondSocketSignalSlot() {
    connect(tcpSocket, &QAbstractSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error) {
        if (error != QAbstractSocket::RemoteHostClosedError)
            emit socketError(tcpSocket->errorString());
        else
            emit socketClosed();
    });
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Peer::handleDataIn);
}

void Peer::sendInializePack() {
    handleDataOut("0" + QString(flipPieceColor(selfColor)));
}

Piece_Color Peer::getSelfColor() {
    return selfColor;
}
