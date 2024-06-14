# Chess



# Design

## Engine

- Board: 8x8 Cells (array\[64\]) store pointers to Pieces (position对应的piece)
- GameState: WhiteWin / BlackWin / WhiteCheckmated / BlackCheckmated / Draw / Unfinished
- White King Position: check whether game is over
- Black King Position: check whether game is over

```c++
// click a piece and only enable the possible cell of the board
QList<Position> getPossibleMove(Position pos){
    Piece* p = getPiece(pos);
    return p->getPossibleMove(board, state, pos);
}

// "to" is definitively the valid Position (unless hacked😅)
GameState nextState(Position from, Position to){
    movePiece(from, to); // 同时要处理吃子，以及Pawn的Promotion
    state = checkGameState();
    return state;
}
```

## Piece

- Piece_Color: White / Black

derived class: Pawn, King, Queen, Rook, Bishop, Knight

```c++
// 根据board中8x8的站位、当前的state，过滤可行的走位
// - 不能跨越到其他棋子后方
// - 可以走到对方棋子上，不能走到自己棋子上
// - Pawn的En passant
// - King的Castling
// - 被将军时必须的自保行为
QList<Position> getPossibleMove(board, pos);
```
