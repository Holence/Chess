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
    bool operator==(const Position &that) const {
        return x == that.x and y == that.y;
    }
    QString toString();
    static Position fromString(QString s);
    static Position fromIndex(int index);
    bool isValid();
    int toIndex() const;
    Position flipSide();
};

#include <QSet>
inline uint qHash(const Position &key, uint seed = 0) {
    return qHash(key.toIndex(), seed);
}

#endif // POSITION_H
