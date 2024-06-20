#ifndef PEERWINDOW_H
#define PEERWINDOW_H

#include "network/peer.h"
#include "widget/basemainwindow.h"

class PeerWindow : public BaseMainWindow {
public:
    PeerWindow(QWidget *parent, bool server);
    ~PeerWindow();

private slots:
    void pieceMovedSlot(Position pos_from, Position pos_to, Piece_Type promoteType);

private:
    bool connectSuccessed();
    bool isServer;
    Peer *peer;
};

#endif // PEERWINDOW_H
