#include "pawn.h"

Pawn::Pawn(Piece_Color color, Position pos, Direction direction) : Piece(color, pos) {
    type = Piece_Type::Pawn;
    this->direction = direction;
    if (direction == Direction::Up) {
        moved = pos.y != 2;
    } else {
        moved = pos.y != 7;
    }
}

QList<Position> Pawn::getAttackMove() {
    QList<Position> l;
    Position p;
    p = Position{pos.x - 1, pos.y + 1 * direction};
    if (isValidPos(p))
        l.append(p);
    p = Position{pos.x + 1, pos.y + 1 * direction};
    if (isValidPos(p))
        l.append(p);

    return l;
}

QList<Position> Pawn::getAdditionMove() {
    QList<Position> l;
    Position p;
    p = Position{pos.x, pos.y + 1 * direction};
    if (isValidPos(p))
        l.append(p);

    if (!moved) {
        l.append(Position{pos.x, pos.y + 2 * direction});
    }
    return l;
}

void Pawn::setMoved() {
    moved = true;
}

bool Pawn::isReadyToPromote() {
    if (direction == Direction::Up) {
        return pos.y == 7;
    } else {
        return pos.y == 2;
    }
}
