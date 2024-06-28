#ifndef PEERWINDOW_H
#define PEERWINDOW_H

#include "network/peer.h"
#include "widget/basemainwindow.h"
#include "widget/chatbox.h"
#include <QSettings>

class PeerWindow : public BaseMainWindow {
public:
    PeerWindow(QWidget *parent, bool server, bool RTS_mode);
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
    bool connectDialog(bool RTS_mode);
    Peer *peer;
    Piece_Color currentColor; // 目前轮到的颜色

    QString tauntCountry;
    QList<QAction *> tauntActionList;
    void playTaunt(QString country, int i);

    QSettings *settings;
};

#endif // PEERWINDOW_H
