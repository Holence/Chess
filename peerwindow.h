#ifndef PEERWINDOW_H
#define PEERWINDOW_H

#include "network/peer.h"
#include "widget/basemainwindow.h"

class PeerWindow : public BaseMainWindow {
public:
    PeerWindow(QWidget *parent, bool server);
    ~PeerWindow();

protected slots:
    void pieceMovedSlot(Movement m);

    void socketErrorSlot(QString error);
    void receivedMovementSlot(Movement m);
    void receivedResignSlot();

    void gameEndSlot(GameState state);

private:
    bool isServer;
    bool connectDialog();
    Peer *peer;
    Piece_Color currentColor; // 目前轮到的颜色
};

#endif // PEERWINDOW_H
