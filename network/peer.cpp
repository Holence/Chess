#include "peer.h"

Peer::Peer(QObject *parent)
    : QObject{parent} {}

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

void Peer::handleDataIn() {
    QString s = QString::fromUtf8(tcpSocket->readAll());
    QString op = s.mid(1);
    switch (s.at(0).digitValue()) {
    case 0:
        if (!initialized) {
            initialized = true;
            // client收到自己的颜色
            if (op == 'w')
                selfColor = Piece_Color::White;
            else
                selfColor = Piece_Color::Black;
            // 发回给server进行确认
            handleDataOut("0" + QString(flipPieceColor(selfColor)));
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

Piece_Color Peer::getSelfColor() {
    return selfColor;
}
