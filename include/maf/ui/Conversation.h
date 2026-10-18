#ifndef CONVERSATION_H
#define CONVERSATION_H

#include <QWidget>
#include <QString>
#include <QTextBrowser>
#include <QTextCursor>
#include <QVBoxLayout>

class QTextBrowser;

class Conversation : public QWidget
{
    Q_OBJECT

public:
    explicit Conversation(QWidget *parent = nullptr);

    void _append_user_message(const QString &content);
    void _append_ai_message(const QString &content);

private:
    void _append_message(const QString &speaker,
                         const QString &content,
                         bool is_user);

    QTextBrowser *m_conversation_view;
};

#endif