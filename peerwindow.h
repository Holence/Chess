#ifndef PEERWINDOW_H
#define PEERWINDOW_H

#include "network/peer.h"
#include "widget/basemainwindow.h"
#include "widget/chatbox.h"

class PeerWindow : public BaseMainWindow {
public:
    PeerWindow(QWidget *parent, bool server);
    ~PeerWindow();

protected slots:
    void pieceMovedSlot(Movement m);

    void socketClosedSlot();
    void receivedMovementSlot(Movement m);
    void receivedResignSlot();

    void gameEndSlot(GameState state);

private:
    ChatBox *chatbox;
    bool isServer;
    bool connectDialog();
    Peer *peer;
    Piece_Color currentColor; // 目前轮到的颜色

    QString tauntCountry;
    QList<QAction *> tauntActionList;
    void playTaunt(QString country, int i);
};

#endif // PEERWINDOW_H
