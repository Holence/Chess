#ifndef BOARD_H
#define BOARD_H

#include "cellbutton.h"
#include "engine/engine.h"
#include <QAction>
#include <QMediaPlayer>
#include <QWidget>

class Board : public QWidget {
    Q_OBJECT
public:
    Board(QWidget *parent, Piece_Color selfColor, bool isPlayingMode);
    ~Board();

    static const QMap<Piece_Type, QString> WhiteIcon, BlackIcon;

    // 单人模拟中要用
    void flipSelfColor();

    // Replay模式的movement、联机模式对方的move -> 传入board -> 传入engine
    void movePiece(Movement m);

    // Ctrl+F
    void flipBoard();

    void playMedia(QString path);

    void setNeedConfirm(bool needConfirm);
    bool getNeedConfirm();

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
    void paintTrace();

private:
    Engine engine;

    Piece_Color selfColor;
    bool boardFilpped;

    void updateCellIcon(Position pos);
    void drawBoard();
    void drawBoardBlind();

    CellButton **cellArray = nullptr;
    CellButton *getCellBtn(Position pos);

    void movePieceHandler(Movement m, bool isMyOperation);
    Piece_Type getPawnPromotion();

    CellButton *selectedCell = nullptr;
    Piece *selectedPiece = nullptr;
    QList<CellButton *> movableCellList;            // 当前选中的棋子的可行走位
    QList<QPair<CellButton *, bool>> traceCellList; // 对方上一步的起点和终点（RTS_Mode中，如果位于黑影中就不要显示，bool会记录是否需要显示，正常模式中bool变量没有任何作用）

    Position translatePos(Position pos);
    QList<Position> translatePosList(QList<Position> posList);

    QMediaPlayer *mediaPlayer;

    bool needConfirm = false;
    bool getConfirm(Movement m);
};

#endif // BOARD_H
