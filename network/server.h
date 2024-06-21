#ifndef SERVER_H
#define SERVER_H
#include "peer.h"

class Server : public Peer {
    Q_OBJECT
public:
    Server(QObject *parent);

private:
    void connectToPeer();
};

#endif // SERVER_H
