#ifndef BOARD_H
#define BOARD_H

#include "cellbutton.h"
#include "engine/engine.h"
#include <QAction>
#include <QWidget>

class Board : public QWidget {
    Q_OBJECT
public:
    Board(QWidget *parent, Piece_Color selfColor, bool replayMode = false);
    ~Board();

    static const QMap<Piece_Type, QString> WhiteIcon, BlackIcon;

    // 单人模拟中要用
    void flipSelfColor();

    // Replay模式的movement、联机模式对方的move -> 传入engine
    void movePiece(Position pos_from, Position pos_to, Piece_Type promoteType);

signals:
    // 己方棋子移动后触发，外界绑定后续的操作（add replay、socket、转换到bot……）
    void pieceMoved(Position pos_from, Position pos_to, Piece_Type promoteType);
    // 棋局结束后触发
    void gameEnded(GameState state);
    // 棋子移动后，存在被吃掉的棋子，召唤主界面绘制
    void pieceEaten(Piece *p_eaten);

private slots:
    void cellSelected(Position pos);
    void cellCanceled();

private:
    QAction actionFlip_Board;

    Engine engine;

    Piece_Color selfColor;
    bool boardFilpped;

    void updateCellIcon(Position pos);
    void drawBoard();

    CellButton **cellArray;
    CellButton *getCellBtn(Position pos);

    Piece_Type getPawnPromotion();

    CellButton *selectedCell;
    Piece *selectedPiece;
    QList<CellButton *> movableCellList;
    QList<CellButton *> filteredCellList;

    Position translatePos(Position pos);
    QList<Position> translatePosList(QList<Position> posList);
};

#endif // BOARD_H
