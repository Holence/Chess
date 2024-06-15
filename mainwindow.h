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
    void cellClicked(Position pos);
    void cellCanceled(Position pos);

private:
    Ui::MainWindow *ui;
    QList<CellButton *> cellList;
    Engine engine;
};
#endif // MAINWINDOW_H
