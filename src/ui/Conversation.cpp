#include "maf/ui/Conversation.h"

namespace
{
    QString _escape_lines(const QString &content)
    {
        return content.toHtmlEscaped().replace("\n", "<br>");
    }
}

Conversation::Conversation(QWidget *parent)
    : QWidget(parent),
      m_conversation_view(new QTextBrowser(this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(18, 12, 18, 8);
    layout->setSpacing(0);

    m_conversation_view->setObjectName("conversationView");
    m_conversation_view->setOpenExternalLinks(true);
    m_conversation_view->setReadOnly(true);

    layout->addWidget(m_conversation_view);
}

void Conversation::_append_user_message(const QString &content)
{
    _append_message("You: ", content, true);
}

void Conversation::_append_ai_message(const QString &content)
{
    _append_message("AI Agent", content, false);
}

void Conversation::_append_message(const QString &speaker,
                                   const QString &content,
                                   bool is_user)
{
    const QString escaped_content = _escape_lines(content);

    QString html;

    if (is_user)
    {
        html = QString(
            "<div style='margin: 18px 4px 22px 80px; text-align: right;'>"
            "<div style='color: #8aa392; font-size: 11px; margin-bottom: 7px;'>%1</div>"
            "<span style='background-color: #20392b; color: #e6eee9; "
            "padding: 11px 15px; border-radius: 15px; display: inline-block; "
            "text-align: left;'>%2</span></div>")
            .arg(speaker.toHtmlEscaped(), escaped_content);
    }
    else
    {
        html = QString(
            "<div style='margin: 18px 80px 22px 4px;'>"
            "<div style='color: #6ee7a8; font-size: 12px; font-weight: bold; "
            "margin-bottom: 7px;'>✦ %1</div>"
            "<div style='color: #dce7df; line-height: 1.5;'>%2</div></div>")
            .arg(speaker.toHtmlEscaped(), escaped_content);
    }

    QTextCursor cursor = m_conversation_view->textCursor();

    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock();
    cursor.insertHtml(html);
    cursor.insertBlock();

    m_conversation_view->setTextCursor(cursor);
    m_conversation_view->ensureCursorVisible();
}
