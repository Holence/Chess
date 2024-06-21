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
    virtual int getPort() = 0;

    void disconnectSocket();

    void sendMovement(Movement m);
    void sendResign();

    Piece_Color getSelfColor();

signals:
    void socketError(QString error);
    void connectionSuccessed();

    void receivedMovement(Movement m);
    void receivedResign();
public slots:
    void setSelfColor(Piece_Color color);

protected:
    bool initialized = false;
    Piece_Color selfColor;
    void handleDataIn();
    void handleDataOut(QString s);
    QTcpServer *tcpServer = nullptr;
    QTcpSocket *tcpSocket = nullptr;
};

#endif // PEER_H
