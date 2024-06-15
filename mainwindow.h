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

private slots:
    void cellSelected(Position pos);
    void cellCanceled();

private:
    Ui::MainWindow *ui;
    Engine engine;

    QMap<Piece_Type, QString> WhiteIcon, BlackIcon;
    void setCellIcon(Position pos, Piece *p);
    void clearCellIcon(Position pos);

    CellButton **board;
    CellButton *getCell(Position pos);

    CellButton *selectedCell;
    QList<CellButton *> highlightCellList;
};
#endif // MAINWINDOW_H
