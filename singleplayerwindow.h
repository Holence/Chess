#ifndef SINGLEPLAYERWINDOW_H
#define SINGLEPLAYERWINDOW_H

#include "widget/basemainwindow.h"

class SinglePlayerWindow : public BaseMainWindow {
    Q_OBJECT
public:
    SinglePlayerWindow(QWidget *parent, bool RTS_mode);

protected slots:
    void pieceMovedSlot(Movement m);
};
#endif // SINGLEPLAYERWINDOW_H
