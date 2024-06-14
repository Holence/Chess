#include "position.h"

bool isValidPos(Position pos) {
    return pos.x >= 1 and pos.x <= 8 and pos.y >= 1 and pos.y <= 8;
}
