#include "position.h"

bool Position::isValid() {
    return x >= 1 and x <= 8 and y >= 1 and y <= 8;
}

int Position::toIndex() const {
    return x - 1 + (y - 1) * 8;
}

Position Position::fromIndex(int index) {
    return Position{(index % 8) + 1, (index / 8) + 1};
}

Position Position::flipSide() {
    return Position{9 - x, 9 - y};
}

QString Position::toString() {
    return QString("%1%2").arg(QChar('a' + x - 1)).arg(y);
}

Position Position::fromString(QString s) {
    return Position{s.at(0).unicode() - 'a' + 1, s.at(1).digitValue()};
}
