#ifndef CLIENT_H
#define CLIENT_H
#include "peer.h"

class Client : public Peer {
    Q_OBJECT
public:
    Client(QObject *parent);
    void connectToServer(QString &hostName, quint16 port);

private:
};

#endif // CLIENT_H
