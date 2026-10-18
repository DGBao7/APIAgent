#include "maf/ui/InputBar.h"

InputBar::InputBar(QWidget *parent)
    : QWidget(parent),
      m_input_field(new QLineEdit(this))
{
    setObjectName("inputPanel");

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(22, 14, 22, 18);
    layout->setSpacing(0);

    m_input_field->setObjectName("inputField");
    m_input_field->setPlaceholderText("Nhập tin nhắn rồi nhấn Enter...");
    m_input_field->setMinimumHeight(48);
    m_input_field->setClearButtonEnabled(false);

    layout->addWidget(m_input_field);

    connect(m_input_field, &QLineEdit::returnPressed,
            this, &InputBar::_submit_message);
}

void InputBar::_submit_message()
{
    const QString message = m_input_field->text().trimmed();

    if (message.isEmpty())
    {
        return;
    }

    m_input_field->clear();
    emit message_submitted(message);
}
