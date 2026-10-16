#include "maf/core/Agent.h"
#include "maf/core/Result.h"
#include "maf/providers/ILLMProvider.h"

<<<<<<< Updated upstream
namespace maf::core 
{
    Agent::Agent(std::string id, std::string name)
        : agentId(id), agentName(name) {}

    Agent::~Agent() 
    {
        // Destructor ảo (có thể để trống nếu không có tài nguyên động cần cấp phát/thu hồi thủ công)
=======
#include <vector>

namespace maf::core
{
    Agent::Agent(
        std::string id, std::string name ,
        maf::providers::ILLMProvider *provider
    )
    {
        this -> agentId = std::move(id);
        this -> agentName = std::move(name);
        this -> provider = provider;
>>>>>>> Stashed changes
    }
    
    maf::core::Result Agent::performTask(
        maf::core::Task& task
    )
    {
        std::vector<maf::providers::Message> messages = {
            {"system", "You are a helpful AI agent."},
            {"user", task.getDesc()}
        };

<<<<<<< Updated upstream
    std::string Agent::getId() const 
    {
        return agentId;
    }

    std::string Agent::getName() const 
    {
        return agentName;
    }
} 
=======
        const auto response = provider->_chat(messages);

        if (!response.ok)
        {
            return maf::core::Result(false, "", response.error);
        }

        return maf::core::Result(true, response.text, "");
    }

    Agent::~Agent() = default;

    std::string Agent::getId() const
    {
        return agentId;
    }

    std::string Agent::getName() const
    {
        return agentName;
    }

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

    Result Agent::execute(Task& task)
    {
        const std::string validationError = validateTask(task);
        if (!validationError.empty()) 
        {
            if (task.getStatus() == TaskStatus::PENDING) {
                task.setStatus(TaskStatus::FAILED);
            }
            return Result(false, "", validationError);
        }

        try 
        {
            task.setStatus(TaskStatus::RUNNING);

            Result result = performTask(task);

            task.setStatus(result.isSuccess()
                            ? TaskStatus::COMPLETED
                            : TaskStatus::FAILED);
            return result;
        } 
        catch (const std::exception& e) 
        {
            task.setStatus(TaskStatus::FAILED);
            return Result(false, "", e.what());
        } 
        catch (...) 
        {
            task.setStatus(TaskStatus::FAILED);
            return Result(false, "", "Unknown error during Agent execution.");
        }
    }

    void Agent::_workflow(maf::core::Task task)
    {
        std::cout << "===== Execute =====" << "\n";
        maf::core::Result result = this -> execute(task);
        result._print_result();

        std::cout << "===== Is validate =====" << "\n";
        std::string is_validate = this -> validateTask(task);

        if (is_validate == "")
        {
            this -> performTask(task);
        }

        return;
    }
}
>>>>>>> Stashed changes
