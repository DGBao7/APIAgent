#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace maf::core
{
    class Task;
};

namespace maf::agents::strategy 
{ 
    class LLMStrategyContext;
}

namespace maf::orchestration 
{ 
    class Memory;

    struct PlanStep {
        int index;
        std::string toolName;
        std::string instruction;
    };

    class Planner {
    public:
        std::vector<PlanStep> createPlan(
            const maf::core::Task& task,
            maf::agents::strategy::LLMStrategyContext& llmContext,
            Memory& memory) const;
    };
} 
