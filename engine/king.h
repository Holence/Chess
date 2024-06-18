#ifndef KING_H
#define KING_H

#include "piece.h"

class King : public Piece {

public:
    King(Piece_Color color, Position pos);
    QList<Position> getAttackMove();
    void setMoved();
    bool getMoved();

private:
    bool moved;
};

#endif // KING_H
