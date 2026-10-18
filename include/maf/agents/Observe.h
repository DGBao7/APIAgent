#ifndef OBSERVE_H
#define OBSERVE_H

#include <string>
#include <vector>
#include <cstddef>
#include <algorithm>
#include <cctype>
#include <stdexcept>

#include "maf/core/Agent.h"
#include "maf/core/Task.h"
#include "maf/core/Result.h"

#include "maf/agents/Strategy.h"

#include "maf/orchestration/Planner.h"
#include "maf/orchestration/Memory.h"
#include "maf/orchestration/Executor.h"

namespace maf::core
{
    struct Observation 
    {
        std::string goal;
        std::vector<std::string> requirements;
        std::vector<std::string> constraints;
    };

    struct AgentMessage
    {
        std::string sender;
        std::string receiver;
        std::string content;
    };

    class Observer
    {
    private:
        std::vector<AgentMessage> messages_;

    public:
        Observation parseInput(const std::string &raw_input);

        double measureSystemLoad(
            std::size_t message_queue_size,
            std::size_t active_sub_agents
        );

        void _send_message(
            const std::string &sender,
            const std::string &receiver,
            const std::string &content,
            maf::orchestration::Memory &memory
        );

        const std::vector<AgentMessage> &_get_messages() const;

        maf::core::Result _coordinate_agents(
            maf::core::Agent &researcher,
            maf::core::Agent &planner_agent,
            maf::core::Task &task,
            maf::agents::strategy::Strategy &strategy,
            maf::agents::strategy::LLMStrategyContext &context,
            maf::orchestration::Planner &planner,
            maf::orchestration::Executor &executor,
            maf::orchestration::Memory &memory
        );
    };
} 

#endif