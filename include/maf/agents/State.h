
#ifndef STATE_H
#define STATE_H

#include <atomic>
#include <string>
#include <utility>

namespace maf::agents
{
    enum class AgentState
    {
        IDLE,
        THINKING,
        EXECUTING,
        COMPLETED,
        FAILED
    };

    struct AgentStatus
    {
        std::string name;
        std::atomic<AgentState> state = AgentState::IDLE;

        explicit AgentStatus(std::string name)
            : name(std::move(name))
        {
        }
    };
}

#endif
