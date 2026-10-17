#ifndef MAF_CORE_AGENT_H
#define MAF_CORE_AGENT_H

#include <string>
#include <algorithm>
#include <cctype>
#include <exception>
#include <utility>
#include <vector>

#include "maf/core/Result.h"
#include "maf/core/Task.h"

#include "maf/providers/ILLMProvider.h"

#include "maf/agents/Strategy.h"

#include "maf/orchestration/Planner.h"
#include "maf/orchestration/Executor.h"
#include "maf/orchestration/Memory.h"

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
        ~Agent();
        
        std::string getId() const;
        std::string getName() const;

        Result performTask(Task task);

        std::string validateTask(const Task &task) const;

        bool _check_task_status(Task &task);

        Result _workflow(
            Task &task,
            maf::agents::strategy::Strategy &strategy,
            maf::agents::strategy::LLMStrategyContext &context,
            maf::orchestration::Planner &planner,
            maf::orchestration::Executor &executor,
            maf::orchestration::Memory &memory
        );
    };
};

#endif