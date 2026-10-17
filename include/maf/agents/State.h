#ifndef STATE_H
#define STATE_H

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
}

#endif