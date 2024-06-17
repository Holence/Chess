#ifndef KING_H
#define KING_H

#include "piece.h"

class King : public Piece {

public:
    King(Piece_Color color, Position pos);
    QList<Position> getAttackMove();
    QList<Position> getAdditionMove();
    void setMoved();

private:
    bool moved;
};

#endif // KING_H
