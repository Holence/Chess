#ifndef BASEMAINWINDOW_H
#define BASEMAINWINDOW_H

#include "ui_basewindow.h"
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

protected:
    Ui::MainWindow *ui;
};

#endif // BASEMAINWINDOW_H
