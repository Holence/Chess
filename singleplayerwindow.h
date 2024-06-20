#ifndef SINGLEPLAYERWINDOW_H
#define SINGLEPLAYERWINDOW_H

#include "widget/basemainwindow.h"

class SinglePlayerWindow : public BaseMainWindow {
    Q_OBJECT
public:
    SinglePlayerWindow(QWidget *parent);

private slots:
    void pieceMovedSlot(Position pos_from, Position pos_to, Piece_Type promoteType);
};
#endif // SINGLEPLAYERWINDOW_H
