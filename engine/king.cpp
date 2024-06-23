#include "king.h"

King::King(Piece_Color color, Position pos) : Piece(color, pos) {
    type = Piece_Type::king;
    moved = pos.x != 5;
    if (!moved) {
        if (color == Piece_Color::White) {
            moved = pos.y != 1;
        } else {
            moved = pos.y != 8;
        }
    }
}

QList<Position> King::getAttackMove() {
    QList<Position> l;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 and j == 0)
                continue;
            else {
                Position p{pos.x + i, pos.y + j};
                if (p.isValid())
                    l.append(p);
            }
        }
    };
    return l;
}

void King::setMoved() {
    moved = true;
}

bool King::getMoved() {
    return moved;
}
