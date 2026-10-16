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

        std::string validateTask(const Task& task) const;

        Result performTask(Task& task);
    public:
        Agent(
            std::string id, std::string name ,
            maf::providers::ILLMProvider *provider
        );

        ~Agent();
        std::string getId() const;
        std::string getName() const;
        Result execute(Task& task);

        void _workflow(maf::core::Task task);
    };
}

#endif
