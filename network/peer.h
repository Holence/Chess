#ifndef PEER_H
#define PEER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class Peer : public QObject {
    Q_OBJECT
public:
    explicit Peer(QObject *parent = nullptr);
    virtual int getPort() = 0;
signals:
    void socketError(QString error);
    void connectSuccessed();

protected:
    void handleDataIn();
    void handelDataOut(QString s);
    QTcpServer *tcpServer = nullptr;
    QTcpSocket *tcpSocket = nullptr;
};

#endif // PEER_H
