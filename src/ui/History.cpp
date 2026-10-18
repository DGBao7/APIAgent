
#include "maf/ui/History.h"

History::History(QWidget *parent)
    : QWidget(parent),
      m_history_list(new QListWidget(this)),
      m_status_timer(new QTimer(this))
{
    setObjectName("historyPanel");
    setMinimumWidth(225);
    setMaximumWidth(310);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 24, 16, 18);
    layout->setSpacing(12);

    QLabel *title = new QLabel("AGENT STATUS", this);
    title->setObjectName("sectionTitle");

    QLabel *description = new QLabel(
        "Trạng thái thực tế của Agent", this
    );
    description->setObjectName("mutedText");

    m_history_list->setObjectName("historyList");
    m_history_list->setSelectionMode(
        QAbstractItemView::NoSelection
    );

    layout->addWidget(title);
    layout->addWidget(description);
    layout->addWidget(m_history_list, 1);

    connect(
        m_status_timer,
        &QTimer::timeout,
        this,
        &History::_update_agent_statuses
    );

    m_status_timer->start(200);

    _update_agent_statuses();
}

void History::_set_agent_statuses(
    const std::vector<
        std::shared_ptr<maf::agents::AgentStatus>
    > &statuses
)
{
    m_agent_statuses = statuses;

    _update_agent_statuses();
}

void History::_update_agent_statuses()
{
    m_history_list->clear();

    if (m_agent_statuses.empty())
    {
        m_history_list->addItem("Agent: Not connected");
        return;
    }

    for (const auto &agent_status : m_agent_statuses)
    {
        if (!agent_status)
        {
            continue;
        }

        QString status;

        switch (agent_status->state.load())
        {
            case maf::agents::AgentState::IDLE:
                status = "Idle";
                break;

            case maf::agents::AgentState::THINKING:
                status = "Thinking";
                break;

            case maf::agents::AgentState::EXECUTING:
                status = "Executing";
                break;

            case maf::agents::AgentState::COMPLETED:
                status = "Completed";
                break;

            case maf::agents::AgentState::FAILED:
                status = "Failed";
                break;
        }

        const QString text = QString("%1: %2")
            .arg(QString::fromStdString(agent_status->name))
            .arg(status);

        m_history_list->addItem(text);
    }
}
