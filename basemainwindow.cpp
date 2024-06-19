#include "basemainwindow.h"

BaseMainWindow::BaseMainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
}

BaseMainWindow::~BaseMainWindow() {
    delete ui;
}
