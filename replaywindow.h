#ifndef REPLAYWINDOW_H
#define REPLAYWINDOW_H

#include "replay.h"
#include "widget/basemainwindow.h"

class ReplayWindow : public BaseMainWindow {
public:
    ReplayWindow(QWidget *parent, QString &filename);
    ~ReplayWindow();

private:
    Piece_Color selfColor;

    QList<Replay::Movement> movementList;

    int replayIndex;
    void ReplayNextMove();

    QAction actionNext_Move;
};

#endif // REPLAYWINDOW_H