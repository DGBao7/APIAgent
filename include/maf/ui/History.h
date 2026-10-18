
#ifndef HISTORY_H
#define HISTORY_H

#include "maf/agents/State.h"

#include <QAbstractItemView>
#include <QLabel>
#include <QListWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <memory>
#include <vector>

class History : public QWidget
{
    Q_OBJECT

public:
    explicit History(QWidget *parent = nullptr);

    void _set_agent_statuses(
        const std::vector<
            std::shared_ptr<maf::agents::AgentStatus>
        > &statuses
    );

private:
    QListWidget *m_history_list;
    QTimer *m_status_timer;

    std::vector<
        std::shared_ptr<maf::agents::AgentStatus>
    > m_agent_statuses;

    void _update_agent_statuses();
};

#endif
