#include "basemainwindow.h"
#include <QLabel>

BaseMainWindow::BaseMainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
}

BaseMainWindow::~BaseMainWindow() {
    delete ui;
}

void BaseMainWindow::addPieceEaten(Piece *p_eaten) {
    QLabel *label = new QLabel(this);
    label->setFixedSize(QSize(25, 25));
    if (p_eaten->getColor() == Piece_Color::White) {
        label->setPixmap(QPixmap(Board::WhiteIcon.value(p_eaten->getType())).scaled(25, 25, Qt::KeepAspectRatio));
        ui->horizontalLayout_white_eaten->addWidget(label);
    } else {
        label->setPixmap(QPixmap(Board::BlackIcon.value(p_eaten->getType())).scaled(25, 25, Qt::KeepAspectRatio));
        ui->horizontalLayout_black_eaten->addWidget(label);
    }
}

void BaseMainWindow::bondBoardSlot() {
    connect(ui->actionFilp_Board, &QAction::triggered, board, &Board::flipBoard);
}
