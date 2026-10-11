#ifndef AGENT_H
#define AGENT_H

#include <string>
#include "maf/core/Result.h"
#include "maf/core/Task.h"

namespace maf::core {

class Agent {
    private:
        std::string agentId;
        std::string agentName;

    public:
        Agent(std::string id, std::string name);
        virtual ~Agent();

        std::string getId() const;
        std::string getName() const;
        
        virtual Result execute(Task& task) = 0;
};

}

#endif