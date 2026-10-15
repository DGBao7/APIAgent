#ifndef MAF_CORE_AGENT_H
#define MAF_CORE_AGENT_H

#include <string>
#include "maf/core/Result.h"
#include "maf/core/Task.h"

namespace maf::core {

class Agent {
private:
    std::string agentId;
    std::string agentName;
    std::string validateTask(const Task& task) const;
protected:
    virtual Result performTask(Task& task) = 0;
public:
    Agent(std::string id, std::string name);
    virtual ~Agent();
    std::string getId() const;
    std::string getName() const;
    Result execute(Task& task);
};

} 

#endif 
