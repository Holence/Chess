#include "chatbox.h"
#include <QVBoxLayout>

ChatBox::ChatBox(const QString &title, QWidget *parent) : QDockWidget(title, parent) {
    setFloating(true);
    setFeatures(features() & ~QDockWidget::DockWidgetClosable);
    setAllowedAreas(Qt::NoDockWidgetArea);

    QWidget *multiWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(multiWidget);

    messageDisplay = new QPlainTextEdit(this);
    messageDisplay->setReadOnly(true);
    messageDisplay->appendPlainText("Communication channel established.\n"
                                    "Input message and press Enter to send.\n"
                                    "Note that the channel is not encrypted.\n"
                                    "So be careful what you say.\n"
                                    "-----------------------------------------------------\n");

    QFont f("Microsoft YaHei UI", 13, QFont::Medium);
    messageDisplay->setFont(f);
    layout->addWidget(messageDisplay);

    messageInput = new QLineEdit(this);
    messageInput->setFont(f);
    connect(messageInput, &QLineEdit::returnPressed, this, [this] {
        QString s(messageInput->text());
        if (!s.isEmpty()) {
            messageDisplay->appendPlainText(QString("%1: %2\n").arg(selfName, s));
            emit sendMessage(s);
            messageInput->clear();
        }
    });
    layout->addWidget(messageInput);

    multiWidget->setLayout(layout);
    setWidget(multiWidget);

    show();
}

void ChatBox::receivedMessage(QString s) {
    messageDisplay->appendPlainText(QString("%1: %2\n").arg(oppName, s));
}

void ChatBox::setSelfName(QString newSelfName) {
    selfName = newSelfName;
}

void ChatBox::setOppName(QString newOppName) {
    oppName = newOppName;
}
