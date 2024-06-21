#include "chatbox.h"
#include <QVBoxLayout>

ChatBox::ChatBox(const QString &title, QWidget *parent) : QDockWidget(title, parent) {
    setFloating(true);
    setFeatures(features() & ~QDockWidget::DockWidgetClosable);
    setAllowedAreas(Qt::NoDockWidgetArea);
    resize(QSize(300, 700));

    QWidget *multiWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(multiWidget);

    messageDisplay = new QTextEdit(this);
    messageDisplay->setReadOnly(true);
    QFont f("Microsoft YaHei UI", 14, QFont::Medium);
    messageDisplay->setFont(f);
    layout->addWidget(messageDisplay);

    messageInput = new QLineEdit(this);
    messageInput->setFont(f);
    connect(messageInput, &QLineEdit::returnPressed, this, [this] {
        QString s = messageInput->text();
        if (!s.isEmpty()) {
            messageDisplay->append("Me: " + s + "\n");
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
    messageDisplay->append("Opponent: " + s + "\n");
}
