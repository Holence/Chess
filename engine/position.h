#ifndef POSITION_H
#define POSITION_H

// y
// 8 . . . . . . . .
// 7 . . . . . . . .
// 6 . . . . . . . .
// 5 . . . . . . . .
// 4 . . . . . . . .
// 3 . . . . . . . .
// 2 . . . . . . . .
// 1 . . . . . . . .
//   1 2 3 4 5 6 7 8 x
// board index 从左下角开始往右递增，再往上递增

struct Position {
    int x, y;
};

bool isValidPos(Position pos);

#endif // POSITION_H
