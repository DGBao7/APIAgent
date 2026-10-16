#ifndef MAF_CORE_AGENT_H
#define MAF_CORE_AGENT_H

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
    public:
        Agent(
            std::string id, std::string name ,
            maf::providers::ILLMProvider *provider
        );

        Result performTask(Task task);
        ~Agent();
        std::string getId() const;
        std::string getName() const;
        Result execute(Task task);

        std::string validateTask(const Task task) const;

        void _workflow(maf::core::Task task);
    };
};

#endif