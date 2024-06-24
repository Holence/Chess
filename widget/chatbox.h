#ifndef CHATBOX_H
#define CHATBOX_H

#include <QDockWidget>
#include <QLineEdit>
#include <QPlainTextEdit>

class ChatBox : public QDockWidget {
    Q_OBJECT
public:
    ChatBox(const QString &title, QWidget *parent = nullptr);
    void receivedMessage(QString s);
    void setSelfName(QString newSelfName);
    void setOppName(QString newOppName);

signals:
    QString sendMessage(QString s);

private:
    QPlainTextEdit *messageDisplay;
    QLineEdit *messageInput;
    QString selfName, oppName;
};

#endif // CHATBOX_H
