#ifndef CELLBUTTON_H
#define CELLBUTTON_H

#include "engine/position.h"
#include <QMouseEvent>
#include <QPushButton>

class CellButton : public QPushButton {
    Q_OBJECT
public:
    CellButton(Position pos);

signals:
    void rightClicked();
    void leftClicked(Position pos);
private slots:
    void mousePressEvent(QMouseEvent *e);

private:
    Position pos;
};

#endif // CELLBUTTON_H
