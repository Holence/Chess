#ifndef KNIGHT_H
#define KNIGHT_H

#include "piece.h"

class Knight : public Piece {
public:
    Knight(Piece_Color color, Position pos);
    QList<Position> getAttackMove();
};

#endif // KNIGHT_H
