#include "cellbutton.h"

CellButton::CellButton(Position pos) {
    this->pos = pos;
    setIconSize(QSize(100, 100));
    setFixedSize(QSize(100, 100));
    clearColor();
}

Position CellButton::getPos() {
    return pos;
}

void CellButton::paintColor(int r, int g, int b, int a) {
    setStyleSheet(QString("background-color: rgba(%1,%2,%3,%4);").arg(r).arg(g).arg(b).arg(a));
}

void CellButton::clearColor() {
    setStyleSheet("background-color: rgba(0,0,0,0);");
}

void CellButton::mousePressEvent(QMouseEvent *e) {
    if (e->button() == Qt::RightButton) {
        emit rightClicked();
    } else if (e->button() == Qt::LeftButton) {
        emit leftClicked(pos);
    }
    QPushButton::mousePressEvent(e);
}
