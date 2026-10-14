#include "maf/core/Agent.h"

namespace maf::core {

Agent::Agent(std::string id, std::string name)
    :agentId(id), agentName(name) {}

Agent::~Agent() {}

std::string Agent::getId() const {return agentId;}
std::string Agent::getName() const {return agentName;}

} 