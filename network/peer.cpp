#include "peer.h"
#include <QDateTime>

Peer::Peer(QObject *parent)
    : QObject{parent} {
}

QString Peer::getConnectionInfo() {
    return tcpSocket->localAddress().toString() + ":" + QString::number(tcpSocket->localPort()) + " -> " + tcpSocket->peerAddress().toString() + ":" + QString::number(tcpSocket->peerPort());
}

void Peer::disconnectSocket() {
    if (tcpSocket and tcpSocket->isOpen())
        tcpSocket->close();
}

void Peer::sendMovement(Movement m) {
    handleDataOut("5" + Movement::toString(m));
}

void Peer::sendResign() {
    handleDataOut("9");
}

void Peer::sendMessage(QString s) {
    handleDataOut("8" + s);
}

void Peer::sendTaunt(QString country, int i) {
    handleDataOut("7" + country + " " + QString::number(i));
}

void Peer::sendTime() {
    handleDataOut("1" + QString::number(QDateTime::currentMSecsSinceEpoch()));
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
                // server开启timer
                timer->start();
                sendTime();
            } else {
                emit socketError("Color not match!!!");
            }
        }
        break;
    case 1:
        if (!isServer) {
            // client收到server发来的时间戳，向server返回自己的时间
            emit receivedTime(QDateTime::currentMSecsSinceEpoch() - op.toLongLong());
            sendTime();
        } else {
            // 收到client的时间戳，结束
            emit receivedTime(QDateTime::currentMSecsSinceEpoch() - op.toLongLong());
        }
        break;
    case 5:
        emit receivedMovement(Movement::fromString(op));
        break;
    case 7:
        emit receivedTaunt(op.split(" ").at(0), op.split(" ").at(1).toInt());
        break;
    case 8:
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
