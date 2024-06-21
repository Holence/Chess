#ifndef BOARD_H
#define BOARD_H

#include "cellbutton.h"
#include "engine/engine.h"
#include <QAction>
#include <QWidget>

class Board : public QWidget {
    Q_OBJECT
public:
    Board(QWidget *parent, Piece_Color selfColor, bool isPlayingMode = true);
    ~Board();

    static const QMap<Piece_Type, QString> WhiteIcon, BlackIcon;

    // 单人模拟中要用
    void flipSelfColor();

    // Replay模式的movement、联机模式对方的move -> 传入board -> 传入engine
    void movePiece(Movement m);

    // Ctrl+F
    void flipBoard();
signals:
    // 己方棋子移动后触发，外界绑定后续的操作（to replay、to socket、转换到bot……）
    void pieceMoved(Movement m);
    // 棋局结束后触发（在basemainwindow中有默认的实现
    void gameEnded(GameState state);
    // 棋子移动后，存在被吃掉的棋子，召唤主界面绘制（在basemainwindow中有默认的实现
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

    CellButton **cellArray = nullptr;
    CellButton *getCellBtn(Position pos);

    Piece_Type getPawnPromotion();

    CellButton *selectedCell = nullptr;
    Piece *selectedPiece = nullptr;
    QList<CellButton *> movableCellList; // 当前选中的棋子的可行走位
    QList<CellButton *> traceCellList;   // 对方上一步的起点和终点

    Position translatePos(Position pos);
    QList<Position> translatePosList(QList<Position> posList);
};

#endif // BOARD_H
