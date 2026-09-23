#pragma once

#include <string>
#include <vector>

namespace maf::agents::strategy { 
class LLMStrategyContext;
}

class Task; 

namespace maf::orchestration { 

class Memory;

struct PlanStep {
    int index;
    std::string toolName;
    std::string instruction;
};

class Planner {
public:
    std::vector<PlanStep> createPlan(const Task& task,
                                      maf::agents::strategy::LLMStrategyContext& llmContext,
                                      Memory& memory) const;
};

} 
