#ifndef PEER_H
#define PEER_H
#include "engine/piece.h"
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

class Peer : public QObject {
    Q_OBJECT
public:
    explicit Peer(QObject *parent = nullptr);
    QString getConnectionInfo();

    void disconnectSocket();

    void sendReadyToReceiveSignal();
    void sendMovement(Movement m);
    void sendResign();
    void sendMessage(QString s);
    void sendTaunt(QString country, int i);
    void sendPing();
    void sendNickname();

    int getPort();
    Piece_Color getSelfColor();
    QString getNickname();

signals:
    void socketError(QString error);
    void socketClosed();
    void connectionSuccessed();

    void receivedMovement(Movement m);
    void receivedResign();
    void receivedMessage(QString s);
    void receivedTaunt(QString country, int i);
    void receivedTime(int latency);

    void receivedOppNickname(QString nickname);
public slots:
    void setSelfColor(Piece_Color color);
    void setNickname(QString nickname);

protected:
    bool isServer;
    Piece_Color selfColor;
    void bondSocketSignalSlot();
    void sendInializePack();
    void handleDataIn();
    void handleDataOut(QString s);
    QTcpServer *tcpServer = nullptr;
    QTcpSocket *tcpSocket = nullptr;
    QTimer *timer;

    QString nickname;
};

#endif // PEER_H
