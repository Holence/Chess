#include "chatbox.h"
#include <QVBoxLayout>

ChatBox::ChatBox(const QString &title, QWidget *parent) : QDockWidget(title, parent) {
    setFloating(true);
    setFeatures(features() & ~QDockWidget::DockWidgetClosable);
    setAllowedAreas(Qt::NoDockWidgetArea);
    resize(QSize(400, 800));

    QWidget *multiWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(multiWidget);

    messageDisplay = new QPlainTextEdit(this);
    messageDisplay->setReadOnly(true);
    messageDisplay->appendHtml("<p style=\"font-size:10pt; color:gray;\">Communication channel established.<br>"
                               "Input message and press Enter to send.<br>"
                               "Note that the channel is not encrypted, so be careful what you say.<br>"
                               "---------------------------------------------------------<br></p>");

    QFont f("Microsoft YaHei UI", 13, QFont::Medium);
    messageDisplay->setFont(f);
    layout->addWidget(messageDisplay);

    messageInput = new QLineEdit(this);
    messageInput->setFont(f);
    connect(messageInput, &QLineEdit::returnPressed, this, [this] {
        QString s(messageInput->text());
        if (!s.isEmpty()) {
            messageDisplay->appendPlainText("Me: " + s + "\n");
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
    messageDisplay->appendPlainText("Opponent: " + s + "\n");
}
