#include "state.h"

namespace maf::agents {


const char* toString(AgentState state) {
    switch (state) {
        case AgentState::IDLE:
            return "IDLE";
        case AgentState::THINKING:
            return "THINKING";
        case AgentState::EXECUTING:
            return "EXECUTING";
        case AgentState::COMPLETED:
            return "COMPLETED";
        case AgentState::FAILED:
            return "FAILED";
        default:
            return "UNKNOWN";
    }
}

} 
