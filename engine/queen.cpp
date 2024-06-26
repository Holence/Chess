#include "queen.h"

Queen::Queen(Piece_Color color, Position pos) : Piece(color, pos) {
    type = Piece_Type::queen;
}

QList<Position> Queen::getAttackMove() {
    QList<Position> l;

    int offset[3] = {-1, 0, 1};
    for (int i = 0; i < 3; i++) {
        int x_offset = offset[i];

        for (int j = 0; j < 3; j++) {
            int y_offset = offset[j];

            if (x_offset == 0 and y_offset == 0)
                continue;

            for (int x = pos.x + x_offset, y = pos.y + y_offset; true; x += x_offset, y += y_offset) {
                Position pos_check{x, y};
                if (pos_check.isValid()) {
                    l.append(pos_check);
                } else {
                    break;
                }
            }
        }
    }
    return l;
}
