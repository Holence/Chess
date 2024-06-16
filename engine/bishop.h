#ifndef BISHOP_H
#define BISHOP_H

#include "piece.h"

class Bishop : public Piece {
public:
    Bishop(Piece_Color color, Position pos);
    QList<Position> getAttackMove();
};

#endif // BISHOP_H
