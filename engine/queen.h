#ifndef QUEEN_H
#define QUEEN_H

#include "piece.h"

class Queen : public Piece {
public:
    Queen(Piece_Color color, Position pos);
    QList<Position> getPossibleMove();
};

#endif // QUEEN_H
