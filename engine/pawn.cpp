#include "pawn.h"

Pawn::Pawn(Piece_Color color, Position pos, Direction direction) : Piece(color, pos) {
    type = Piece_Type::Pawn;
    moved = false;
    this->direction = direction;
}

QList<Position> Pawn::getAttackMove() {
    // 这里只给出斜吃，无奈Engine调用Piece->getAttackMove的地方只有getSuppressingPos()中，这里就狗皮膏药一下……
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
