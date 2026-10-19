#include "maf/utils/Env.h"

#include "maf/providers/GroqProvider.h"
#include "maf/providers/OpenRouterProvider.h"
#include "maf/providers/ILLMProvider.h"

#include "maf/core/Task.h"
#include "maf/core/Agent.h"
#include "maf/core/Result.h"

#include "maf/agents/Strategy.h"
#include "maf/agents/Observe.h"

#include "maf/orchestration/Planner.h"
#include "maf/orchestration/Executor.h"
#include "maf/orchestration/Memory.h"

#include "maf/ui/MainWindow.h"

#include <QApplication>
#include <QCoreApplication>
#include <QMetaObject>
#include <QString>

#include <atomic>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include <functional>
#include <windows.h>

std::vector<maf::providers::ILLMProvider *> _start_API()
{
    const bool loaded = maf::utils::Env::_load(".env");

    if (!loaded)
    {
        std::cerr
            << "Can not load .env from: "
            << std::filesystem::current_path()
            << "\n";
    }
    else
    {
        std::cerr << "Env loaded successfully\n";
    }

    static maf::providers::GroqProvider groq;
    static maf::providers::OpenRouterProvider open_router;

    return {
        &groq,
        &open_router
    };
}

std::vector<maf::core::Agent> _start_agents(
    const std::vector<maf::providers::ILLMProvider *> &providers
)
{
    const std::vector<std::string> agents_name = {
        "Researcher",
        "Planner"
    };

    if (providers.size() < agents_name.size())
    {
        throw std::runtime_error(
            "The multi-agent workflow requires at least two LLM providers."
        );
    }

    std::vector<maf::core::Agent> agents;
    agents.reserve(agents_name.size());

    for (size_t i = 0; i < agents_name.size(); i++)
    {
        agents.emplace_back(
            std::to_string(i),
            agents_name[i],
            providers[i]
        );
    }

    return agents;
}

class LLMChatTool : public maf::orchestration::ITool
{
private:
    maf::agents::strategy::LLMStrategyContext &context;

public:
    explicit LLMChatTool(
        maf::agents::strategy::LLMStrategyContext &context
    ) : context(context)
    {
    }

    std::string getName() const override
    {
        return "llm chat tool";
    }

    std::string execute(const std::string &input) override
    {
        const std::vector<maf::providers::Message> messages = {
            {
                "system",
                "You are my assistant. Be who you are and give me the best"
            },
            {"user", input}
        };

        const size_t provider_count = context.getProviderNames().size();
        std::string last_error;

        for (size_t i = 0; i < provider_count; i++)
        {
            const auto provider = context.getProvider();

            if (!provider)
            {
                break;
            }

            const auto response = provider->_chat(
                messages,
                0.2,
                context.getMaxTokens()
            );

            if (response.ok)
            {
                return response.text;
            }

            last_error = response.error;

            if (i + 1 < provider_count)
            {
                context.switchToNextProvider();
            }
        }

        throw std::runtime_error(
            "All LLM providers failed. Last error: " + last_error
        );
    }
};

std::string _join_items(const std::vector<std::string> &items)
{
    std::string joined;

    for (const auto &item : items)
    {
        if (!joined.empty())
        {
            joined += ", ";
        }

        joined += item;
    }

    return joined.empty() ? std::string("None") : joined;
}

std::string _process_request(
    const std::string &input,
    const std::vector<maf::providers::ILLMProvider *> &providers,
    const std::function<void(
        const std::vector<std::shared_ptr<maf::agents::AgentStatus>> &
    )> &on_statuses_ready
)
{
    if (providers.size() < 2)
    {
        throw std::runtime_error(
            "The multi-agent workflow requires at least two LLM providers."
        );
    }

    maf::core::Observer observe;
    const maf::core::Observation observation = observe.parseInput(input);
    maf::core::Task task("001", observation.goal);

    std::vector<std::shared_ptr<maf::providers::ILLMProvider>> converted_providers =
        maf::agents::strategy::borrowProviders(providers);

    maf::agents::strategy::LLMStrategyContext context(
        converted_providers,
        1024,
        3,
        4096
    );

    std::vector<maf::core::Agent> agents = _start_agents(providers);

    std::vector<std::shared_ptr<maf::agents::AgentStatus>> agent_statuses;

    for (const auto &agent : agents)
    {
        agent_statuses.push_back(agent._get_status());
    }

    on_statuses_ready(agent_statuses);

    maf::agents::strategy::LLMSelectionStrategy strategy(*providers[0]);

    maf::orchestration::Planner planner;
    maf::orchestration::Executor executor;
    maf::orchestration::Memory memory;

    memory.setFact("observed_goal", observation.goal);
    memory.setFact(
        "observed_requirements",
        _join_items(observation.requirements)
    );
    memory.setFact(
        "observed_constraints",
        _join_items(observation.constraints)
    );

    executor.registerTool(
        std::make_shared<LLMChatTool>(context)
    );

    maf::core::Result result = observe._coordinate_agents(
        agents[0],
        agents[1],
        task,
        strategy,
        context,
        planner,
        executor,
        memory
    );

    if (result.isSuccess())
    {
        return result.getData();
    }

    return "Error: " + result.getError();
}

int main(int argc, char *argv[])
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    QApplication app(argc, argv);
    app.setApplicationName("APIAgent");

    std::vector<maf::providers::ILLMProvider *> providers;

    try
    {
        providers = _start_API();
    }
    catch (const std::exception &error)
    {
        std::cerr << "Provider initialization failed: " << error.what() << '\n';
        return 1;
    }

    MainWindow window;
    window.show();

    std::thread worker;
    std::atomic_bool request_in_progress = false;

    QObject::connect(
        &window,
        &MainWindow::user_message_submitted,
        &window,
        [&](const QString &message)
        {
            bool expected = false;

            if (!request_in_progress.compare_exchange_strong(expected, true))
            {
                return;
            }

            window._set_busy(true);

            if (worker.joinable())
            {
                worker.join();
            }

            try
            {
                worker = std::thread(
                    [&window, &providers, &request_in_progress, message]()
                    {
                        QString answer;

                        try
                        {
                            answer = QString::fromStdString(
                                _process_request(
                                    message.toStdString(),
                                    providers,
                                    [&window](
                                        const std::vector<
                                            std::shared_ptr<maf::agents::AgentStatus>
                                        > &statuses
                                    )
                                    {
                                        const auto agent_statuses = statuses;

                                        QMetaObject::invokeMethod(
                                            &window,
                                            [&window, agent_statuses]()
                                            {
                                                window._set_agent_statuses(agent_statuses);
                                            },
                                            Qt::QueuedConnection
                                        );
                                    }
                                )
                            );
                        }
                        catch (const std::exception &error)
                        {
                            answer = QString("Error: %1").arg(
                                QString::fromUtf8(error.what())
                            );
                        }
                        catch (...)
                        {
                            answer = "Error: An unknown error occurred.";
                        }

                        QMetaObject::invokeMethod(
                            &window,
                            [&window, &request_in_progress, answer]()
                            {
                                window._append_ai_message(answer);
                                window._set_busy(false);
                                request_in_progress.store(false);
                            },
                            Qt::QueuedConnection
                        );
                    }
                );
            }
            catch (const std::exception &error)
            {
                window._append_ai_message(
                    QString("Could not start request: %1").arg(
                        QString::fromUtf8(error.what())
                    )
                );
                window._set_busy(false);
                request_in_progress.store(false);
            }
        }
    );

    QObject::connect(
        &app,
        &QCoreApplication::aboutToQuit,
        &app,
        [&worker]()
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
    );

    return app.exec();
}