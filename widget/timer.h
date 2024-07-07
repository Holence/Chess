#ifndef TIMER_H
#define TIMER_H

#include <QLabel>
#include <QTime>

class Timer : public QLabel {
    Q_OBJECT

public:
    Timer(QTime total_time);
    ~Timer();
    void stop();
    void resume();
    void setTime(QTime newTime);
    QTime getTime() const;

signals:
    void timeup();

private:
    QTimer *seconder;
    QTime time_left;
    void tick();
};
#endif // TIMER_H
