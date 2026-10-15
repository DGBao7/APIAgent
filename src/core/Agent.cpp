#include "maf/core/Agent.h"

#include <algorithm>
#include <cctype>
#include <exception>
#include <utility>

namespace maf::core {

Agent::Agent(std::string id, std::string name)
    :agentId(std::move(id)), agentName(std::move(name)) {}

Agent::~Agent() = default;

std::string Agent::getId() const {return agentId;}
std::string Agent::getName() const {return agentName;}

std::string Agent::validateTask(const Task& task) const 
    {
    if (task.getStatus() != TaskStatus::PENDING) {return "Task must be PENDING before execution.";}
    const auto hasNonWhitespace = [](const std::string& value) {
        return std::any_of(value.begin(), value.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        });
    };

    if (!hasNonWhitespace(task.getId())) {
        return "Task ID cannot be empty or whitespace only.";
    }

    if (!hasNonWhitespace(task.getDesc())) {
        return "Task description cannot be empty or whitespace only.";
    }

        return "";
    }

Result Agent::execute(Task& task) {
    const std::string validationError = validateTask(task);
    if (!validationError.empty()) {
        if (task.getStatus() == TaskStatus::PENDING) {
            task.setStatus(TaskStatus::FAILED);
        }
        return Result(false, "", validationError);
    }

    try {
        task.setStatus(TaskStatus::RUNNING);

        Result result = performTask(task);

        task.setStatus(result.isSuccess()
                        ? TaskStatus::COMPLETED
                        : TaskStatus::FAILED);
        return result;
    } catch (const std::exception& e) {
        task.setStatus(TaskStatus::FAILED);
        return Result(false, "", e.what());
    } catch (...) {
        task.setStatus(TaskStatus::FAILED);
        return Result(false, "", "Unknown error during Agent execution.");
    }
}

} 
