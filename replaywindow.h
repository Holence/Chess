#ifndef REPLAYWINDOW_H
#define REPLAYWINDOW_H

#include "widget/basemainwindow.h"

class ReplayWindow : public BaseMainWindow {
public:
    ReplayWindow(QWidget *parent, QString &filename);

private:
    QList<Movement> movementList;

    int replayIndex;
    void ReplayNextMove();

    QAction actionNext_Move;
};

#endif // REPLAYWINDOW_H
