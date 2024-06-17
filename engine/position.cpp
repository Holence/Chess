#include "position.h"

bool isValidPos(Position pos) {
    return pos.x >= 1 and pos.x <= 8 and pos.y >= 1 and pos.y <= 8;
}

int convertPosToIndex(Position pos) {
    return pos.x - 1 + (pos.y - 1) * 8;
}

Position convertIndexToPos(int index) {
    return Position{(index % 8) + 1, (index / 8) + 1};
}

Position flipSide(Position pos) {
    return Position{9 - pos.x, 9 - pos.y};
}
