#include "maf/core/Agent.h"

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
    }
    
    Result Agent::performTask(
        Task task
    )
    {
        std::vector<maf::providers::Message> messages = {
            {"system", "You are a helpful AI agent."},
            {"user", task.getDesc()}
        };

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

    maf::providers::ILLMProvider *Agent::_get_provider() const
    {
        return this -> provider;
    }

    bool Agent::_check_task_status(Task &task)
    {
        if (task.getStatus() != TaskStatus::PENDING)
        {
            return false;
        }

        task.setStatus(TaskStatus::RUNNING);

        return true;
    }

    std::string Agent::validateTask(const Task &task) const
    {
        if (task.getStatus() != TaskStatus::PENDING) 
        {
            return "Task must be PENDING before execution.";
        }
        
        const auto hasNonWhitespace = [](const std::string& value) 
        {
            return std::any_of(value.begin(), value.end(), [](unsigned char ch) 
            {
                return !std::isspace(ch);
            });
        };

        if (!hasNonWhitespace(task.getDesc()))
        {
            return "You need to say something";
        }

        return "";
    }

    maf::agents::AgentState Agent::_get_state() const noexcept
    {
        return this -> agent_state;
    }

    // agents gửi câu hỏi đến từng api riêng
    maf::providers::LLMResponse Agent::_run(
        std::string prompt ,
        std::string input
    )
    {
        this -> agent_state = maf::agents::AgentState::THINKING;

        try
        {
            std::vector<maf::providers::Message> messages = {
                {"system" , prompt} ,
                {"user" , input}
            };

            maf::providers::LLMResponse response =
                this -> provider -> _chat(messages);

            if (response.ok)
            {
                this -> agent_state = maf::agents::AgentState::COMPLETED;
            }
            else
            {
                this -> agent_state = maf::agents::AgentState::FAILED;
            }

            return response;
        }
        catch (const std::exception &error)
        {
            this -> agent_state = maf::agents::AgentState::FAILED;

            maf::providers::LLMResponse response;
            response.ok = false;
            response.error = error.what();

            return response;
        }
    } 

    Result Agent::_workflow(
        Task &task,
        maf::agents::strategy::Strategy &strategy,
        maf::agents::strategy::LLMStrategyContext &context,
        maf::orchestration::Planner &planner,
        maf::orchestration::Executor &executor,
        maf::orchestration::Memory &memory
    )
    {
        const std::string validation_error = this -> validateTask(task);

        if (!validation_error.empty())
        {
            if (task.getStatus() == TaskStatus::PENDING)
            {
                task.setStatus(TaskStatus::FAILED);
            }

            return Result(false , "" , validation_error);
        }

        if (!this -> _check_task_status(task))
        {
            return Result(
                false , 
                "" ,
                "Task must be PENDING before execution."
            );
        }

        try
        {
            const auto decision = strategy.decide(task , context);

            // std::cout << "\n===== Strategy =====\n";
            // std::cout << "Strategy result:\n";
            // std::cout << "Success: " << decision.isSuccess << '\n';
            // std::cout << "Selected provider: "
                    // << decision.selectedProvider << '\n';
            // std::cout << "Max tokens: "
                    // << decision.maxTokens << '\n';

            // std::cout << "Provider ranking:\n";

            // for (const auto &name : decision.providerRanking)
            // {
            //     std::cout << "- " << name << '\n';
            // }

            // std::cout << "Used fallback: "
                    // << decision.usedFallback << '\n';

            // std::cout << "Error: "
            //         << decision.errorMessage << '\n';

            if (!decision.isSuccess)
            {
                task.setStatus(TaskStatus::FAILED);

                const std::string error = decision.errorMessage.empty()
                    ? "Strategy failed to select a provider."
                    : decision.errorMessage;

                return Result(false , "" , error);
            }

            maf::providers::LLMResponse research_result;

            if (this -> getName() == "Planner")
            {
                research_result = maf::providers::LLMResponse{
                    .ok = true,
                    .text = memory.getFact("research_result")
                };
            }

            // std::cout << "\n[DEBUG] Strategy passed\n";

            const std::vector<std::string> tool_names = executor.getToolRepository().listTools();

            // std::cout << "[DEBUG] Registered tools: "
                    // << tool_names.size() << '\n';

            // for (const auto &name : tool_names)
            // {
            //     // std::cout << "[DEBUG] Tool: " << name << '\n';
            // }

            if (tool_names.empty())
            {
                task.setStatus(TaskStatus::FAILED);

                return Result(
                    false,
                    "",
                    "Executor has no tools registered."
                );
            }

            std::string available_tools;

            for (const auto &tool_name : tool_names)
            {
                if (!available_tools.empty())
                {
                    available_tools += ", ";
                }

                available_tools += tool_name;
            }

            memory.setFact("available_tools" , available_tools);

            memory.setFact("available_tools", available_tools);

// std::cout << "[DEBUG] Calling Planner\n";
            this -> agent_state = maf::agents::AgentState::THINKING;

            const auto plan = planner.createPlan(
                task , 
                context ,
                memory
            );

            std::string plan_context;

            for (const auto &step : plan)
            {
                plan_context +=
                    "Step " + std::to_string(step.index)
                    + ": " + step.toolName
                    + "\nInstruction: " + step.instruction
                    + "\n\n";
            }

            memory.setFact("plan", plan_context);

// std::cout << "[DEBUG] Planner returned "
        //   << plan.size() << " steps\n";

// std::cout << "\n===== GENERATED PLAN =====\n";
// for (const auto &step : plan)
// {
//     std::cout << "Step " << step.index << '\n';
//     std::cout << "Tool: " << step.toolName << '\n';
//     std::cout << "Instruction: " << step.instruction << "\n\n";
// }

// std::cout << "[DEBUG] Calling Executor\n";
            this -> agent_state = maf::agents::AgentState::EXECUTING;

            Result result = executor.executePlan(plan , memory);

            task.setStatus(
                result.isSuccess()
                    ? TaskStatus::COMPLETED
                    : TaskStatus::FAILED
            );

// std::cout << "[DEBUG] Executor finished\n";
// std::cout << "[DEBUG] Success: " << result.isSuccess() << '\n';
// std::cout << "[DEBUG] Error: " << result.getError() << '\n';

            if (!result.isSuccess())
            {
                return result;
            }         
            
            this -> agent_state = maf::agents::AgentState::THINKING;

            // đọc memory
            const std::string memory_context = memory.buildContext();

            std::vector<maf::providers::Message> messages = {
                {
                    "system",
                    "You are a helpful AI agent. "
                    "Answer the user's request using the tool execution results "
                    "provided in the context. Do not invent results."
                },
                {
                    "user",
                    "Original request: " + task.getDesc()
                    + "\n\nExecution context:\n" + memory_context
                    + "\n\nProvide the final answer to the user."
                }
            };

            const auto provider = context.getProvider();

            if (!provider)
            {
                return Result(
                    false , 
                    result.getData() ,
                    "No active provider available for final response."
                );
            }

            // response
            const auto response = provider -> _chat(messages);

            if (!response.ok)
            {
                return Result(false, result.getData(), response.error);
            }

            memory.addEntry("assistant" , response.text);

            this -> agent_state = maf::agents::AgentState::COMPLETED;

            return Result(true, response.text, "");

            // return result;
        }
        catch (const std::exception &error)
        {
            this -> agent_state = maf::agents::AgentState::FAILED;

            std::cerr << "[ERROR] " << error.what() << '\n';

            task.setStatus(TaskStatus::FAILED);

            return Result(false , "" , error.what());
        }
        catch (...)
        {
            this -> agent_state = maf::agents::AgentState::FAILED;

            task.setStatus(TaskStatus::FAILED);

            return Result(
                false , 
                "" ,
                "Unknown error during Agent workflow."
            );
        }
    }
}
