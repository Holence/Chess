#ifndef PAWN_H
#define PAWN_H

#include "piece.h"

class Pawn : public Piece {
public:
    Pawn(Piece_Color color, Position pos);
    QList<Position> getAttackMove();
    QList<Position> getAdditionMove();
    void setMoved();
    bool getMoved();
    bool isReadyToPromote();

private:
    bool moved;
    int direction;
};

#endif // PAWN_H
