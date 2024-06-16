# Chess

TODO:

- replay file

  `Position from - Position to - Piece_Type eat`

  这样也就支持倒推（悔棋）

- 单人模拟

- 局域网对战

- 人机对战

# Design

## Backend

需要board 8x8 array记录着piece的指针（位置空的话为nullptr），因为界面点击的时候传入的是位置，engine需要快速对应到piece并给出PossibleMove，再次点击另一个位置，给engine传入两个位置from&to，执行movePiece

每个piece也要记录自己的位置，因为计算某个落点施加的压力`caclPressure`时要遍历场上某一方所有的棋子（不光是bot需要，判断King是否被将也需要遍历）

### Engine

- `Piece** board`: 8x8 Cells (array\[64\]) store pointers to Pieces (position对应的piece)
- GameState: WhiteWin / BlackWin / WhiteCheckmated / BlackCheckmated / Draw / Unfinished

- `QList<Piece *> White/BlackPieces、White/BlackDeadPieces`: 记录场上的棋子、吃掉的棋子
- `Piece* White/BlackKing`: 方便每次move后检查state
- `Position LastPawnLongMove`: 如果上一次是对方的Pawn走了两格，且贴着己方的Pawn，则己方的Pawn可以En passant

```c++
// "to" is definitively the valid Position (unless hacked😅)
GameState nextGameState(Position from, Position to){
    movePiece(from, to);
    state = checkGameState();
    return state;
}

// click a piece and only enable the possible cell of the board
QList<Position> getPossibleMove(Position pos){
    Piece* p = getPiece(pos);
    p->getPossibleMove(board, state, pos);
    // 再根据board、当前的state，过滤可行的走位
    // - 不能跨越到其他棋子后方
    // - 可以走到对方棋子上，不能走到自己棋子上
    // - King不能走向对方的势力范围
    // - 被将军时其他兵唯一的自保行为
    // - Pawn是否可斜吃
    // - Pawn是否可En passant
}

void movePiece(Position from, Position to){
    // 同时要处理吃子，以及Pawn的Promotion
}
```

### Piece

- Piece_Color White / Black
- Piece_Type Pawn, King, Queen, Rook, Bishop, Knight
- Position pos

```c++
// 每种兵的基础走法 返回棋盘范围内所有可能的落点
// - King的Castling
QList<Position> getPossibleMove(board, pos);
```

## Frontend

### MainWindow

- Engine
- `CellButton** board`: 8x8 (array\[64\]) 透明按钮
- `CellButton *selectedCell`: 左键后选中的存在走位的Cell，下一次再按左键，且点中了highlightCellList中的某一Cell，则移动棋子engine.nextGameState()
- `QList<CellButton *> highlightCellList`: selectedCell经过getPossibleMove()得到的，在界面上呈现出黄色提示
- `QMap<Piece_Type, QString> WhiteIcon, BlackIcon` 棋子图标

