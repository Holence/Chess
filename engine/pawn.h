#ifndef PAWN_H
#define PAWN_H

#include "piece.h"

class Pawn : public Piece {
public:
    enum Direction : int {
        Up = 1,
        Down = -1
    };
    Pawn(Piece_Color color, Position pos, Direction direction);
    QList<Position> getAttackMove();
    QList<Position> getAdditionMove();
    void setMoved();

private:
    bool moved;
    Direction direction;
};

#endif // PAWN_H
