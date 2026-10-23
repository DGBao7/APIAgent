#include "maf/agents/observe.h"
#include <sstream>
#include <algorithm>

namespace maf::core
{
    bool _hasContent(const std::string &value)
    {
        return std::any_of(
            value.begin(),
            value.end(),
            [](unsigned char c)
            {
                return std::isspace(c) == 0;
            }
        );
    }

    Observation Observer::parseInput(const std::string &raw_input)
    {
        Observation obs;

        obs.goal = raw_input;

        obs.requirements.push_back("Standard execution");
        obs.constraints.push_back("None");

        return obs;
    }

    double Observer::measureSystemLoad(
        std::size_t message_queue_size,
        std::size_t active_sub_agents
    )
    {
        const double load_score =
            (message_queue_size * 1.5)
            + (active_sub_agents * 0.5);

        return load_score;
    }

    void Observer::_send_message(
        const std::string &sender,
        const std::string &receiver,
        const std::string &content,
        maf::orchestration::Memory &memory
    )
    {
        if (
            !_hasContent(sender)
            || !_hasContent(receiver)
            || !_hasContent(content)
        )
        {
            throw std::invalid_argument(
                "agents/observer: receiver and content empty."
            );
        }

        messages_.push_back({
            sender,
            receiver,
            content
        });

        const std::string message_key =
            "agent_message_"
            + std::to_string(messages_.size())
            + "_"
            + sender
            + "_to_"
            + receiver;

        memory.setFact(message_key, content);

        memory.addEntry(
            sender,
            "Message to " + receiver + ":\n" + content
        );
    }

    const std::vector<AgentMessage> &Observer::_get_messages() const
    {
        return messages_;
    }

    maf::core::Result Observer::_coordinate_agents(
        maf::core::Agent &researcher,
        maf::core::Agent &planner_agent,
        maf::core::Task &task,
        maf::agents::strategy::Strategy &strategy,
        maf::agents::strategy::LLMStrategyContext &context,
        maf::orchestration::Planner &planner,
        maf::orchestration::Executor &executor,
        maf::orchestration::Memory &memory
    )
    {
        try
        {
            const auto research_result = researcher._run(
                "You are a Researcher. "
                "Identify the user's intent and provide only concise, relevant facts. "
                "If the request is ambiguous, state the most likely interpretation. "
                "Do not write a long report.",
                task.getDesc()
            );

            if (!research_result.ok)
            {
                task.setStatus(maf::core::TaskStatus::FAILED);

                return maf::core::Result(
                    false,
                    "",
                    "Researcher failed: " + research_result.error
                );
            }

            memory.setFact(
                "research_result",
                research_result.text
            );

            this -> _send_message(
                "Researcher",
                "Planner",
                research_result.text,
                memory
            );

            return planner_agent._workflow(
                task,
                strategy,
                context,
                planner,
                executor,
                memory
            );
        }
        catch (const std::exception &error)
        {
            task.setStatus(maf::core::TaskStatus::FAILED);

            return maf::core::Result(
                false,
                "",
                "Agent coordination failed: "
                    + std::string(error.what())
            );
        }
        catch (...)
        {
            task.setStatus(maf::core::TaskStatus::FAILED);

            return maf::core::Result(
                false,
                "",
                "Agent coordination failed: unknown error."
            );
        }
    }
}