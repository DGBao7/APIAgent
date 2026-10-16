#ifndef AGENT_H
#define AGENT_H

#include <string>
#include <algorithm>
#include <cctype>
#include <exception>
#include <utility>

#include "maf/core/Result.h"
#include "maf/core/Task.h"
#include "maf/providers/ILLMProvider.h"

namespace maf::core
{
    class Agent
    {
    private:
        std::string agentId;
        std::string agentName;
        maf::providers::ILLMProvider *provider;

<<<<<<< Updated upstream
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
=======
        std::string validateTask(const Task& task) const;
>>>>>>> Stashed changes

        Result performTask(Task& task);
    public:
        Agent(
            std::string id, std::string name ,
            maf::providers::ILLMProvider *provider
        );

<<<<<<< Updated upstream
#endif
=======
        ~Agent();
        std::string getId() const;
        std::string getName() const;
        Result execute(Task& task);

        void _workflow(maf::core::Task task);
    };
}

#endif
>>>>>>> Stashed changes
