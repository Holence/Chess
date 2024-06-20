#ifndef BASEMAINWINDOW_H
#define BASEMAINWINDOW_H
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
    explicit BaseMainWindow(QWidget *parent = nullptr);
    ~BaseMainWindow();
    void bondBoardSlot();
public slots:
    void addPieceEaten(Piece *p_eaten);

protected:
    Board *board = nullptr;
    Ui::MainWindow *ui;
};

#endif // BASEMAINWINDOW_H
