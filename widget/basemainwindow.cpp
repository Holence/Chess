#include "basemainwindow.h"
#include <QLabel>
#include <QMessageBox>
#include <QScreen>

BaseMainWindow::BaseMainWindow(QWidget *parent, bool isPlayingMode)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setFixedSize(800, 846);

    // move to center of screen
    move(pos() + (QGuiApplication::primaryScreen()->geometry().center() - geometry().center()));

    setAttribute(Qt::WA_DeleteOnClose);

    this->isPlayingMode = isPlayingMode;

    if (isPlayingMode) {
        actionResign = new QAction(this);
        actionResign->setText("Resign");
        connect(actionResign, &QAction::triggered, this, [this] {
            if (selfColor == Piece_Color::White)
                return gameEndSlot(GameState::BlackWin);
            else
                return gameEndSlot(GameState::WhiteWin);
        });
        ui->menuGame->addAction(actionResign);
    }
}

BaseMainWindow::~BaseMainWindow() {
    delete ui;
    if (replay) {
        replay->replaySave();
        delete replay;
    }
    delete board;
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
    // Ctrl+F 翻转
    connect(ui->actionFilp_Board, &QAction::triggered, board, &Board::flipBoard);
    // 有子被吃，在下面绘制
    connect(board, &Board::pieceEaten, this, &BaseMainWindow::addPieceEaten);
    // 结束的MessageBox
    connect(board, &Board::gameEnded, this, &BaseMainWindow::gameEndSlot);
}

void BaseMainWindow::gameEndSlot(GameState state) {
    if (isPlayingMode) {
        // 保存记录
        replay->replaySave();
        delete replay;
        replay = nullptr;
        actionResign->setDisabled(true);
    }

    board->playMedia("qrc:/sound/game-end.mp3");

    QString s;

    switch (state) {
    case GameState::WhiteWin:
        if (selfColor == Piece_Color::White)
            s = QString("You Win");
        else
            s = QString("You Lose");
        break;
    case GameState::BlackWin:
        if (selfColor == Piece_Color::Black)
            s = QString("You Win");
        else
            s = QString("You Lose");
        break;
    case GameState::Draw:
        s = QString("Draw");
        break;
    default:
        // Unfinished
        s = QString("Opponent Resigned");
    }

    QMessageBox msg(this);
    msg.setWindowTitle("Game Over");
    msg.setText(s);
    msg.adjustSize();
    msg.exec();

    board->setDisabled(true);
}
