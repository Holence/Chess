#ifndef PEERWINDOW_H
#define PEERWINDOW_H

#include "network/peer.h"
#include "widget/basemainwindow.h"
#include "widget/chatbox.h"
#include "widget/timer.h"
#include <QSettings>

class PeerWindow : public BaseMainWindow {
public:
    PeerWindow(QWidget *parent, bool server);
    ~PeerWindow();

protected slots:
    void pieceMovedSlot(Movement m);

    void socketClosedSlot();
    void receivedMovementSlot(Movement m);
    void receivedForceEnd(int reason);
    void gameForceEndSlot(int reason);

private:
    ChatBox *chatbox;
    bool isServer;
    bool openConnectDialog();
    Peer *peer;
    Piece_Color currentColor; // 目前轮到的颜色

    QString tauntCountry;
    QList<QAction *> tauntActionList;
    void playTaunt(QString country, int i);

    QSettings *settings;
    Timer *selfTimer, *oppTimer;
};

#endif // PEERWINDOW_H
