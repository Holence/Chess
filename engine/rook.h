#ifndef ROOK_H
#define ROOK_H

#include "piece.h"

class Rook : public Piece {
public:
    Rook(Piece_Color color, Position pos);
    QList<Position> getAttackMove();
};

#endif // ROOK_H
