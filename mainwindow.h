#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "cellbutton.h"
#include "engine/engine.h"
#include "replay.h"
#include <QMainWindow>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void pieceMoved();
    void gameEnded(GameState state);

private slots:
    void cellSelected(Position pos);
    void cellCanceled();

private:
    Ui::MainWindow *ui;
    Engine engine;

    bool boardFilpped;
    Piece_Color selfColor;

    QMap<Piece_Type, QString> WhiteIcon, BlackIcon;
    void updateCellIcon(Position pos);
    void drawBoard();

    CellButton **cellArray;
    CellButton *getCellBtn(Position pos);

    CellButton *selectedCell;
    Piece *selectedPiece;
    QList<CellButton *> movableCellList;
    QList<CellButton *> filteredCellList;

    Position translatePos(Position pos);
    QList<Position> translatePosList(QList<Position> posList);

    Piece_Type getPawnPromotion();

    Replay *replay;
    void startReplay();
    void ReplayNextMove();
    void ReplayPreviousMove();

    void startSinglePlayerGame();
    void GameOver(GameState state);
};
#endif // MAINWINDOW_H
