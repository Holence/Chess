#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), engine() {
    ui->setupUi(this);

    WhiteIcon[Piece_Type::King] = ":/img/wk.png";
    BlackIcon[Piece_Type::King] = ":/img/bk.png";

    // draw board
    ui->widgetBoard->setFixedSize(QSize(800, 800));
    ui->widgetBoard->setStyleSheet(
        "#widgetBoard {border-image: url(:/img/bg.png) 0 0 0 0 stretch stretch;}"
        "CellButton {background-color: rgba(0,0,0,0);}");
    QGridLayout *grid = new QGridLayout(ui->widgetBoard);
    grid->setMargin(0);
    grid->setSpacing(0);
    ui->widgetBoard->setLayout(grid);

    // dynamic add CellButton
    board = new CellButton *[64];
    for (int x = 1; x <= 8; x++) {
        for (int y = 1; y <= 8; y++) {
            Position pos{x, y};
            CellButton *btn = new CellButton(pos);
            btn->setIconSize(QSize(100, 100));
            btn->setFixedSize(QSize(100, 100));
            connect(btn, &CellButton::leftClicked, this, &MainWindow::cellSelected);
            connect(btn, &CellButton::rightClicked, this, &MainWindow::cellCanceled);
            grid->addWidget(btn, 8 - x, y - 1);
            board[convertPosToIndex(pos)] = btn;

            setCellIcon(pos, engine.getPiece(pos));
        }
    }

    selectedCell = nullptr;
}

MainWindow::~MainWindow() {
    delete ui;
    delete board;
    // CellButton *btn;
    // for (int i = 0; i < 64; i++) {
    //     btn = board[i];
    //     delete btn;
    // }
}

void MainWindow::cellSelected(Position pos) {
    qInfo() << "Enter Selecting" << selectedCell;
    CellButton *current_select_btn = getCell(pos);

    // 之前有选中有效的棋子
    if (selectedCell) {
        // 如果当前点的是可行的走位，则移动棋子
        if (highlightCellList.contains(current_select_btn)) {
            Position orig_pos = selectedCell->getPos();
            GameState state = engine.nextGameState(orig_pos, pos);
            clearCellIcon(orig_pos);
            setCellIcon(pos, engine.getPiece(pos));
            // TODO Handel Game Over
        }
        cellCanceled();
    }
    //
    else {
        // 获取可行的走位
        QList<Position> l = engine.getPossibleMove(pos);
        if (l.isEmpty()) {
            // 没有选中棋子 或 选的棋子没有可行的走位
            selectedCell = nullptr;
        } else {
            selectedCell = current_select_btn;
            foreach (Position pos, l) {
                // 标为黄色
                CellButton *btn = getCell(pos);
                btn->setStyleSheet("background-color: rgba(255,255,0,63);");
                highlightCellList.append(btn);
            }
        }
    }
    qInfo() << "Quit Selecting" << selectedCell;
}

void MainWindow::cellCanceled() {
    qInfo() << "Enter Caneling" << selectedCell;
    if (selectedCell) {
        foreach (CellButton *btn, highlightCellList) {
            btn->setStyleSheet("background-color: rgba(0,0,0,0);");
        }
        highlightCellList.clear();
        selectedCell = nullptr;
    }
}

void MainWindow::setCellIcon(Position pos, Piece *p) {
    if (p) {
        if (p->getColor() == Piece_Color::White) {
            getCell(pos)->setIcon(QIcon(WhiteIcon.value(p->getType(), QString())));
        } else {
            getCell(pos)->setIcon(QIcon(BlackIcon.value(p->getType(), QString())));
        }
    } else {
        getCell(pos)->setIcon(QIcon(QString()));
    }
}

void MainWindow::clearCellIcon(Position pos) {
    getCell(pos)->setIcon(QIcon(QString()));
}

CellButton *MainWindow::getCell(Position pos) {
    return board[convertPosToIndex(pos)];
}
