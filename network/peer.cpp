#include "peer.h"
#include <QDateTime>

Peer::Peer(QObject *parent) : QObject{parent} {
    // 每隔5秒向对方sendPing()，对方收到后原封不动地返回，从而获得RTT
    timer = new QTimer(this);
    timer->setInterval(5 * 1000);
    connect(timer, &QTimer::timeout, this, &Peer::sendPing);
}

QString Peer::getConnectionInfo() {
    return tcpSocket->localAddress().toString() + ":" + QString::number(tcpSocket->localPort()) + " -> " + tcpSocket->peerAddress().toString() + ":" + QString::number(tcpSocket->peerPort());
}

int Peer::getPort() {
    if (isServer)
        return tcpServer->serverPort();
    else
        return tcpSocket->localPort();
}

void Peer::disconnectSocket() {
    if (tcpSocket and tcpSocket->isOpen())
        tcpSocket->close();
}

/**
 * 初始化，互相给对方传对方的color
 */
void Peer::sendInializePack() {
    QString s = "0" + QString(flipPieceColor(selfColor));
#ifndef RTS_MODE
    s += "0";
#else
    s += "1";
#endif
    handleDataOut(s);
}

/**
 * connectionSuccessed后，自己的peerwindow界面生成完毕，调用这个函数，发信息告诉对方可以给自己发送ping和nickname了
 */
void Peer::sendReadyToReceiveSignal() {
    handleDataOut("0");
}

void Peer::sendPing() {
    if (isServer) {
        handleDataOut("1" + QString::number(QDateTime::currentMSecsSinceEpoch()));
    } else {
        handleDataOut("2" + QString::number(QDateTime::currentMSecsSinceEpoch()));
    }
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

void Peer::sendNickname() {
    handleDataOut("3" + nickname);
}

void Peer::handleDataOut(QString s) {
    // 可能一次传输后readAll()进来多个数据组，所以得用特殊字符划清界限，分割出数据包
    // 而且为了escape传输文本中的\n，就用base64把所有数据都编码一下
    // 数据包格式: "base64(utf8(数字开头+数据)) + \n"
    QByteArray package = s.toUtf8().toBase64() + '\n';
    tcpSocket->write(package);
}

void Peer::handleDataIn() {
    QByteArray packages = tcpSocket->readAll();
    QList<QByteArray> packageList = packages.split('\n');
    foreach (QByteArray package, packageList) {
        QString s = QString::fromUtf8(QByteArray::fromBase64(package));
        if (!s.isEmpty()) {
            QString op = s.mid(1);
            switch (s.at(0).digitValue()) {

            case 0: // initial pack

                // op = "[bw][01]"
                if (!op.isEmpty()) {

#ifndef RTS_MODE
                    if (op.at(1) != "0") {
#else
                    if (op.at(1) != "1") {
#endif
                        if (!isServer) {
                            // client收到
                            emit socketError("Game Version does not match!");
                            sendInializePack();
                            tcpSocket->disconnectFromHost();
                        } else {
                            tcpSocket = nullptr;
                        }
                    } else {
                        if (!isServer) {
                            // client收到自己的颜色
                            if (op.at(0) == 'w')
                                setSelfColor(Piece_Color::White);
                            else
                                setSelfColor(Piece_Color::Black);
                            // 发回给server进行确认
                            sendInializePack();
                            emit connectionSuccessed();
                        } else {
                            // server收到颜色确认
                            if (op.at(0) == selfColor) {
                                emit connectionSuccessed();
                            } else {
                                emit socketError("Color not match!!!");
                            }
                        }
                    }
                } else {
                    // op = ""
                    // Ready To Receive
                    timer->start();
                    sendPing();
                    sendNickname();
                }
                break;

            case 1: // ping pack from server
                if (!isServer) {
                    // client收到server发来的ping package，直接返还
                    handleDataOut(s);
                } else {
                    // server收到client返还的ping package，得到RTT
                    emit receivedTime((QDateTime::currentMSecsSinceEpoch() - op.toLongLong()) / 2);
                }
                break;

            case 2: // ping pack from client
                if (isServer) {
                    // server收到client发来的ping package，直接返还
                    handleDataOut(s);
                } else {
                    // client收到server返还的ping package，得到RTT
                    emit receivedTime((QDateTime::currentMSecsSinceEpoch() - op.toLongLong()) / 2);
                }
                break;

            case 3: // nickname
                emit receivedOppNickname(op);
                break;

            case 5: // movement
                emit receivedMovement(Movement::fromString(op));
                break;

            case 7: // taunt
                emit receivedTaunt(op.split(" ").at(0), op.split(" ").at(1).toInt());
                break;

            case 8: // text message
                emit receivedMessage(op);
                break;

            case 9: // resign
                emit receivedResign();
                break;

            default:
                break;
            }
        }
    }
}

void Peer::setSelfColor(Piece_Color color) {
    selfColor = color;
}

void Peer::setNickname(QString nickname) {
    this->nickname = nickname;
}

QString Peer::getNickname() {
    return nickname;
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

Piece_Color Peer::getSelfColor() {
    return selfColor;
}
