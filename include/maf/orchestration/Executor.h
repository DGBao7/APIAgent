#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "maf/orchestration/Planner.h" 
#include "include/maf/core/Result.h"

class Result;

namespace maf::orchestration { 

class Memory;

class ITool {
public:
    virtual ~ITool() = default;
    virtual std::string getName() const = 0;
    virtual std::string execute(const std::string& input) = 0;
};

class ToolRepository {
public:
    void registerTool(std::shared_ptr<ITool> tool);
    std::shared_ptr<ITool> getTool(const std::string& name) const;
    bool hasTool(const std::string& name) const;
    std::vector<std::string> listTools() const;

private:
    std::unordered_map<std::string, std::shared_ptr<ITool>> tools_;
};

class Executor {
public:
    Executor();

    void registerTool(std::shared_ptr<ITool> tool);
    const ToolRepository& getToolRepository() const noexcept;

    Result executePlan(const std::vector<PlanStep>& plan, Memory& memory);

private:
    ToolRepository toolRepository_;
};

} 
