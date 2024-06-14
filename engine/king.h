#ifndef KING_H
#define KING_H

#include "piece.h"

class King : public Piece {
public:
    King();
    QList<Position> getPossibleMove(Piece **board, Position pos);
};

#endif // KING_H
