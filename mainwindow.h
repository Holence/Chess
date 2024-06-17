#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "cellbutton.h"
#include "engine/engine.h"
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
    void drawBoard();
    void SinglePlayerGame();
    void GameOver();

signals:
    void pieceMoved();
    void gameEnded();

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

    CellButton **cellArray;
    CellButton *getCellBtn(Position pos);

    CellButton *selectedCell;
    QList<CellButton *> movableCellList;
    QList<CellButton *> filteredCellList;

    Position translatePos(Position pos);
    QList<Position> translatePosList(QList<Position> posList);
};
#endif // MAINWINDOW_H
