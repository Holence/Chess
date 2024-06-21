#ifndef PEER_H
#define PEER_H
#include "engine/piece.h"
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class Peer : public QObject {
    Q_OBJECT
public:
    explicit Peer(QObject *parent = nullptr);
    QString getConnectionInfo();

    void disconnectSocket();

    void sendMovement(Movement m);
    void sendResign();
    void sendMessage(QString s);
    void sendTaunt(QString country, int i);

    Piece_Color getSelfColor();

signals:
    void socketError(QString error);
    void socketClosed();
    void connectionSuccessed();

    void receivedMovement(Movement m);
    void receivedResign();
    void receivedMessage(QString s);
    void receivedTaunt(QString country, int i);
public slots:
    void setSelfColor(Piece_Color color);

protected:
    bool isServer;
    Piece_Color selfColor;
    void bondSocketSignalSlot();
    void sendInializePack();
    void handleDataIn();
    void handleDataOut(QString s);
    QTcpServer *tcpServer = nullptr;
    QTcpSocket *tcpSocket = nullptr;
};

#endif // PEER_H
