#ifndef TIMER_H
#define TIMER_H

#include <QLabel>
#include <QTime>

class Timer : public QLabel {
    Q_OBJECT

public:
    Timer(int minute, int second);
    ~Timer();
signals:
    void timeup();

private:
    QTime timer;
};
#endif // TIMER_H
