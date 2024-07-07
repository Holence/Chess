#include "timer.h"
#include <QTimer>
Timer::~Timer() {}

void Timer::stop() {
    seconder->stop();
}

void Timer::resume() {
    seconder->start();
}

void Timer::setTime(QTime newTime) {
    time_left = newTime;
}

QTime Timer::getTime() const {
    return time_left;
}

Timer::Timer(QTime total_time) {
    time_left = total_time;

    seconder = new QTimer(this);
    connect(seconder, &QTimer::timeout, this, &Timer::tick);
    seconder->setInterval(1000);

    setText(total_time.toString("mm:ss"));
}

void Timer::tick() {
    time_left = time_left.addSecs(-1);
    setText(time_left.toString("mm:ss"));
    if (time_left.minute() == 0 and time_left.second() == 0) {
        seconder->stop();
        emit timeup();
    }
}
