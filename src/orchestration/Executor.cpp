
#include "maf/orchestration/Executor.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "maf/orchestration/Memory.h"

namespace maf::orchestration {

namespace {


constexpr const char* kRoleTool = "tool";
constexpr const char* kRoleSystem = "system";
constexpr const char* kFactLastOutput = "last_output";

std::string trimCopy(const std::string& s) {
    std::size_t b = 0;
    std::size_t e = s.size();
    while (b < e && std::isspace(static_cast<unsigned char>(s[b])) != 0) ++b;
    while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1])) != 0) --e;
    return s.substr(b, e - b);
}

std::string toLowerCopy(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

std::string stepFactKey(int index) {
    return "step_" + std::to_string(index) + "_output";
}


std::string resolvePlaceholders(const std::string& text, const Memory& memory) {
    std::string out;
    out.reserve(text.size());

    std::size_t pos = 0;
    while (pos < text.size()) {
        const std::size_t open = text.find("{{", pos);
        if (open == std::string::npos) {
            out.append(text, pos, std::string::npos);
            break;
        }
        const std::size_t close = text.find("}}", open + 2);
        if (close == std::string::npos) {
            out.append(text, pos, std::string::npos);
            break;
        }

        out.append(text, pos, open - pos);
        const std::string key = trimCopy(text.substr(open + 2, close - (open + 2)));
        if (memory.hasFact(key)) {
            out += memory.getFact(key);
        } else {
            out.append(text, open, close + 2 - open);  
        }
        pos = close + 2;
    }
    return out;
}


std::shared_ptr<ITool> findTool(const ToolRepository& repo, const std::string& name) {
    if (repo.hasTool(name)) {
        return repo.getTool(name);
    }
    const std::string wanted = toLowerCopy(name);
    for (const std::string& candidate : repo.listTools()) {
        if (toLowerCopy(candidate) == wanted) {
            return repo.getTool(candidate);
        }
    }
    return nullptr;
}


maf::core::Result makeResult(bool success, const std::string& data, const std::string& error) {
    return maf::core::Result(success, data, error);
}

}  

void ToolRepository::registerTool(std::shared_ptr<ITool> tool) {
    
    if (!tool) {
        throw std::invalid_argument("ToolRepository::registerTool: tool is null");
    }
    const std::string name = tool->getName();
    if (name.empty()) {
        throw std::invalid_argument("ToolRepository::registerTool: tool name is empty");
    }
    tools_[name] = std::move(tool);  
}

std::shared_ptr<ITool> ToolRepository::getTool(const std::string& name) const {
    auto it = tools_.find(name);
    if (it != tools_.end()) {
        return it->second;
    }
    return nullptr;
}

bool ToolRepository::hasTool(const std::string& name) const {
    return tools_.find(name) != tools_.end();
}

std::vector<std::string> ToolRepository::listTools() const {
    std::vector<std::string> toolNames;
    toolNames.reserve(tools_.size());
    for (const auto& kv : tools_) {
        toolNames.push_back(kv.first);
    }
    std::sort(toolNames.begin(), toolNames.end());  
    return toolNames;
}



Executor::Executor() : toolRepository_() {}

void Executor::registerTool(std::shared_ptr<ITool> tool) {
    toolRepository_.registerTool(std::move(tool));
}

const ToolRepository& Executor::getToolRepository() const noexcept {
    return toolRepository_;
}


maf::core::Result Executor::executePlan(const std::vector<PlanStep>& plan, Memory& memory) {
    if (plan.empty()) {
        memory.addEntry(kRoleSystem, "[executor] Plan is empty, nothing to execute.");
        return makeResult(false, "", "Plan is empty");
    }

    std::string lastOutput;

    for (const PlanStep& step : plan) {
        const std::string label =
            "step " + std::to_string(step.index) + " (" + step.toolName + ")";

        
        const std::shared_ptr<ITool> tool = findTool(toolRepository_, step.toolName);
        if (!tool) {
            const std::string error = "Tool not found for " + label;
            memory.addEntry(kRoleSystem, "[executor] " + error);
            return makeResult(false, lastOutput, error);
        }

        
        const std::string input = resolvePlaceholders(step.instruction, memory);

        
        std::string output;
        try {
            output = tool->execute(input);
        } catch (const std::exception& ex) {
            const std::string error = label + " failed: " + ex.what();
            memory.addEntry(kRoleSystem, "[executor] " + error);
            return makeResult(false, lastOutput, error);
        } catch (...) {
            const std::string error = label + " failed: unknown error";
            memory.addEntry(kRoleSystem, "[executor] " + error);
            return makeResult(false, lastOutput, error);
        }

        
        memory.addEntry(kRoleTool, label + " -> " + output);
        memory.setFact(stepFactKey(step.index), output);
        memory.setFact(kFactLastOutput, output);
        lastOutput = std::move(output);
    }

    return makeResult(true, lastOutput, "");
}

}  