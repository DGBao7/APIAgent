#include "maf/agents/state.h"
#include <sstream>

namespace maf::agents {

StateManager::StateManager(std::string id) {
    current_state.agent_id = std::move(id);
    current_state.status = AgentStateStatus::IDLE;
}

void StateManager::updateStatus(AgentStateStatus new_status) {
    current_state.status = new_status;
}

void StateManager::setMemory(const std::string& key, const std::string& value) {
    current_state.short_term_memory[key] = value;
}

std::string StateManager::getMemory(const std::string& key) const {
    auto it = current_state.short_term_memory.find(key);
    if (it != current_state.short_term_memory.end()) {
        return it->second;
    }
    return "";
}

void StateManager::appendHistory(const std::string& entry) {
    current_state.long_term_history.push_back(entry);
}


std::string StateManager::serializeContext() const {
    std::ostringstream oss;
    oss << "AgentID:" << current_state.agent_id << ";";
    oss << "MemorySize:" << current_state.short_term_memory.size() << ";";
    
    
    for (const auto& pair : current_state.short_term_memory) {
        oss << pair.first << "=" << pair.second << "|";
    }
    
    return oss.str();
}


void StateManager::deserializeContext(const std::string& serialized_data) {

 
    current_state.short_term_memory["restored_context"] = serialized_data;
}

AgentState StateManager::getState() const {
    return current_state;
}

} 
