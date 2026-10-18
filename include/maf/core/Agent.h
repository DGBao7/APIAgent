#ifndef MAF_CORE_AGENT_H
#define MAF_CORE_AGENT_H

#include <string>
#include <algorithm>
#include <cctype>
#include <exception>
#include <utility>
#include <vector>
#include <memory>

#include "maf/core/Result.h"
#include "maf/core/Task.h"

#include "maf/providers/ILLMProvider.h"

#include "maf/agents/Strategy.h"
#include "maf/agents/State.h"

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

        std::shared_ptr<maf::agents::AgentStatus> agent_status;
    public:
        Agent(
            std::string id, std::string name ,
            maf::providers::ILLMProvider *provider
        );
        ~Agent();
        
        std::string getId() const;
        std::string getName() const;
        maf::providers::ILLMProvider *_get_provider() const;

        Result performTask(Task task);

        std::string validateTask(const Task &task) const;

        bool _check_task_status(Task &task);

        maf::providers::LLMResponse _run(std::string prompt , std::string input);

        Result _workflow(
            Task &task,
            maf::agents::strategy::Strategy &strategy,
            maf::agents::strategy::LLMStrategyContext &context,
            maf::orchestration::Planner &planner,
            maf::orchestration::Executor &executor,
            maf::orchestration::Memory &memory
        );

        std::shared_ptr<maf::agents::AgentStatus> _get_status() const;
        maf::agents::AgentState _get_state() const noexcept;
    };
};

#endif