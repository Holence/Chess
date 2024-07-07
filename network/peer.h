#ifndef PEER_H
#define PEER_H
#include "engine/piece.h"
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTime>
#include <QTimer>

class Peer : public QObject {
    Q_OBJECT
public:
    explicit Peer(QObject *parent = nullptr);
    QString getConnectionInfo();

    void disconnectSocket();

    void sendReadyToReceiveSignal();
    void sendNickname();
    void sendMovement(Movement m);
    void sendForceLose(int reason);
    void sendMessage(QString s);
    void sendTaunt(QString country, int i);
    void sendPing();
    void sendSyncTime(QTime newTime);

    int getPort();

    Piece_Color getSelfColor();
    QString getNickname();
    QTime getTotalTime() const;

signals:
    void socketError(QString error);
    void socketClosed();
    void connectionSuccessed();

    void receivedMovement(Movement m);
    void receivedForceEnd(int reason);
    void receivedMessage(QString s);
    void receivedTaunt(QString country, int i);
    void receivedPing(int latency);
    void receivedSyncTime(QTime newTime);

    void receivedOppNickname(QString nickname);
public slots:
    void setSelfColor(Piece_Color color);
    void setNickname(QString nickname);
    void setTotalTime(QTime newTotalTime);

protected:
    bool isServer;
    Piece_Color selfColor;
    void bondSocketSignalSlot();
    void sendInializePack();
    void handleDataIn();
    void handleDataOut(QString s);
    QTcpServer *tcpServer = nullptr;
    QTcpSocket *tcpSocket = nullptr;
    QTimer *pingTimer;
    QTime total_time;

    QString nickname;
};

#endif // PEER_H
