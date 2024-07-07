#include "timer.h"
#include <QTimer>

Timer::~Timer() {}

Timer::Timer(int minute, int second) {
    timer = QTime(0, minute, second);

    QTimer *seconder = new QTimer(this);
    connect(seconder, &QTimer::timeout, this, [this, seconder] {
        timer = timer.addSecs(-1);
        setText(timer.toString("mm:ss"));
        if (timer.minute() == 0 and timer.second() == 0) {
            seconder->stop();
            emit timeup();
        }
    });
    seconder->start(1000);

    setText(timer.toString("mm:ss"));
}
