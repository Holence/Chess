#ifndef SERVER_H
#define SERVER_H
#include "peer.h"

class Server : public Peer {
    Q_OBJECT
public:
    Server(QObject *parent);
    int getPort();

private:
    void connectToPeer();
};

#endif // SERVER_H
