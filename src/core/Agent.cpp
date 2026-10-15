#include "maf/core/Agent.h"

namespace maf::core 
{
    Agent::Agent(std::string id, std::string name)
        : agentId(id), agentName(name) {}

    Agent::~Agent() 
    {
        // Destructor ảo (có thể để trống nếu không có tài nguyên động cần cấp phát/thu hồi thủ công)
    }

    std::string Agent::getId() const 
    {
        return agentId;
    }

    std::string Agent::getName() const 
    {
        return agentName;
    }
} 