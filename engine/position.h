#ifndef POSITION_H
#define POSITION_H
#include <QString>

// y
// 8 . . . . . . . .
// 7 . . . . . . . .
// 6 . . . . . . . .
// 5 . . . . . . . .
// 4 . . . . . . . .
// 3 . . . . . . . .
// 2 . . . . . . . .
// 1 . . . . . . . .
//   a b c d e f g h x
// board index 从左下角开始往右递增，再往上递增

struct Position {
    int x, y;
    bool operator==(const Position &that) {
        return x == that.x and y == that.y;
    }
    QString toString();
    static Position fromString(QString s);
    static Position fromIndex(int index);
    bool isValid();
    int toIndex();
    Position flipSide();
};

#endif // POSITION_H
