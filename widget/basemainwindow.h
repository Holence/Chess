#ifndef BASEMAINWINDOW_H
#define BASEMAINWINDOW_H
#include "../replay.h"
#include "board.h"
#include "engine/piece.h"
#include "ui_basemainwindow.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class BaseMainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit BaseMainWindow(QWidget *parent, bool isPlayingMode);
    ~BaseMainWindow();
    void bondBoardSlot();
protected slots:
    void addPieceEaten(Piece *p_eaten);
    void gameEndSlot(GameState state);

protected:
    Ui::MainWindow *ui;

    bool isPlayingMode;
    Replay *replay = nullptr;
    Piece_Color selfColor;
    Board *board = nullptr;
    QAction *actionResign;
    QAction *actionConfirmSetting;
};

#endif // BASEMAINWINDOW_H
