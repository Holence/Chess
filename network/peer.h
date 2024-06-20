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
    void sendMovement(Movement m);

signals:
    void socketError(QString error);
    void connectSuccessed();
    void receivedMovement(Movement m);

protected:
    void handleDataIn();
    void handleDataOut(QString s);
    QTcpServer *tcpServer = nullptr;
    QTcpSocket *tcpSocket = nullptr;
};

#endif // PEER_H
