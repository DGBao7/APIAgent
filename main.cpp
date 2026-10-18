// #include "maf/utils/Env.h"

// #include "maf/providers/GeminiProvider.h"
// #include "maf/providers/GroqProvider.h"
// #include "maf/providers/OpenRouterProvider.h"
// #include "maf/providers/ILLMProvider.h"

// #include "maf/communication/Event.h"

// #include "maf/core/Task.h"
// #include "maf/core/Agent.h"
// #include "maf/core/User.h"
// #include "maf/core/Result.h"

// #include "maf/agents/Strategy.h"
// #include "maf/agents/State.h"
// #include "maf/agents/Observe.h"

// #include "maf/orchestration/Planner.h"
// #include "maf/orchestration/Executor.h"
// #include "maf/orchestration/Memory.h"

// #include <iostream>
// #include <vector>
// #include <filesystem>
// #include <string>
// #include <memory>
// #include <stdexcept>
// #include <windows.h>

// std::vector<maf::providers::ILLMProvider*> _start_API()
// {
//     const bool loaded = maf::utils::Env::_load(".env");

//     if (!loaded)
//     {
//         std::cerr << 
//             "Can not load .env from: "
//             << std::filesystem::current_path()
//             << "\n";
//     }
//     else
//     {
//         std::cerr << 
//             "Env loaded successfully" << "\n";
//     }

//     maf::utils::Env::_load(".env"); // Doc .env

//     std::vector<maf::providers::Message> messages = {
//         {"system" , "You are a concise AI agent."} ,
//         {"user" , "Hello. Explain what an agent is in one sentence."}
//     };

//     // static maf::providers::GeminiProvider gemini;
//     static maf::providers::GroqProvider groq;
//     static maf::providers::OpenRouterProvider open_router;

//     std::vector<maf::providers::ILLMProvider*> providers = {
//         // &gemini ,
//         &groq ,
//         &open_router
//     };

//     for (maf::providers::ILLMProvider* provider : providers)
//     {
//         const auto response = provider -> _chat(messages);

//         std::cout << "[" << provider -> _name() << "]\n";

//         if (response.ok)
//         {
//             std::cout << response.text << "\n\n";
//         }
//         else
//         {
//             std::cout << "Error: " << response.error << "\n\n";
//         }
//     }

//     return providers;
// }

// maf::core::User _start_user()
// {
//     return maf::core::User("123" , "Bon");
// }

// std::vector<maf::core::Agent> _start_agents(
//     std::vector<maf::providers::ILLMProvider*> providers
// )
// {
//     std::vector<maf::core::Agent> agents;

//     const std::vector<std::string> agents_name = {
//         "Researcher" ,
//         "Planner"
//     };

//     for (size_t i = 0; i < providers.size(); i ++)
//     {
//         agents.emplace_back(
//             std::to_string(i) ,
//             agents_name[i] ,
//             providers[i]
//         );
//     }

//     return agents;
// }

// // lấy provider được chọn, gửi yêu cầu đến AI, trả giá trị về executor
// // t viết vào đây vì trông orches sợ quá deo dám động vào
// class LLMChatTool : public maf::orchestration::ITool
// {
// private:
//     maf::agents::strategy::LLMStrategyContext *context;
// public:
//     explicit LLMChatTool(
//         maf::agents::strategy::LLMStrategyContext &context
//     )
//     {
//         this -> context = &context;
//     }

//     std::string getName() const override
//     {
//         return "llm chat tool";
//     }

//     std::string execute(const std::string &input) override
//     {
//         std::vector<maf::providers::Message> messages = {
//             {
//                 "system" ,
//                 "You are my assistant. Be who you are and give me the best"
//             } ,
//             {"user" , input}
//         };

//         const auto provider_count = this -> context -> getProviderNames().size();
//         std::string last_error;

//         for (size_t i = 0; i < provider_count; i ++)
//         {
//             const auto provider = this -> context -> getProvider();

//             if (!provider)
//             {
//                 break;
//             }

// // std::cout << "[DEBUG] Provider: "
// //         << provider -> _name() << "\n";

//             const auto response = provider -> _chat(
//                 messages ,
//                 0.2 ,
//                 context -> getMaxTokens()
//             );

// // std::cout << "[DEBUG] Provider: "
// //         << provider -> _name() << "\n";
// // std::cout << "[DEBUG] Response: "
// //           << response.text << "\n";

//             if (response.ok)
//             {
//                 return response.text;
//             }

//             last_error = response.error;

//             if (i + 1 < provider_count)
//             {
//                 context -> switchToNextProvider();
//             }
//         }

//         throw std::runtime_error(
//             "All LLM providers failed. Last error: " + last_error
//         );
//     }
// };

// void _one_run_only()
// {
//     // debug terminal only
//     SetConsoleOutputCP(CP_UTF8);
//     SetConsoleCP(CP_UTF8);

//     // đọc cấu hình api
//     // khởi tại ai providers
//     std::vector<maf::providers::ILLMProvider*> providers = _start_API();

//     // nhận thông tin user
//     maf::core::User user = _start_user();

//     // lấy input
//     maf::communication::Event event;
//     event._input();
// // std::cout << "===== Input =====" << "\n";
// // event._print_input();

//     // lấy input, khởi tạo quan sát, quản lý tải
//     maf::core::Observer observe;

//     const std::string input = event._return_input();

//     const maf::core::Observation observation =
//         observe.parseInput(input);

//     maf::core::Task task("001" , observation.goal);

// std::cout << "===== OBSERVATION =====\n";
// std::cout << "Goal: " << observation.goal << '\n';

// for (const auto& requirement : observation.requirements)
// {
//     std::cout << "Requirement: " << requirement << '\n';
// }

// for (const auto& constraint : observation.constraints)
// {
//     std::cout << "Constraint: " << constraint << '\n';
// }
// // std::cout << "===== Task =====" << "\n";
// // task._print_task();
    
//     // chuyển kiểu dữ liệu để agent dùng
//     std::vector<std::shared_ptr<maf::providers::ILLMProvider>> converted_providers = 
//         maf::agents::strategy::borrowProviders(providers);

//     // tạo context và agent
//     maf::agents::strategy::LLMStrategyContext context(
//         converted_providers ,
//         1024 ,
//         3 ,
//         4096
//     );
//     maf::core::Agent agent("2006" , "Bon" , providers[1]);

//     // tạo strategy
//     maf::agents::strategy::LLMSelectionStrategy strategy(*providers[1]);

//     // tạo planner
//     maf::orchestration::Planner planner;

//     // tạo executor
//     maf::orchestration::Executor executor;

//     // tạo memory
//     maf::orchestration::Memory memory;

//     memory.setFact("observed_goal" , observation.goal);

//     const auto join_items = [](const std::vector<std::string> &items)
//     {
//         std::string joined;

//         for (const auto &item: items)
//         {
//             if (!joined.empty())
//             {
//                 joined += ", ";
//             }

//             joined += item;
//         }

//         return joined.empty() ? std::string("None") : joined;
//     };

//     memory.setFact(
//         "observed_requirements",
//         join_items(observation.requirements)
//     );

//     memory.setFact(
//         "observed_constraints",
//         join_items(observation.constraints)
//     );

//     // đăng ký tool
//     executor.registerTool(
//         std::make_shared<LLMChatTool>(context)
//     );

//     maf::core::Result result = agent._workflow(
//         task ,
//         strategy ,
//         context ,
//         planner ,
//         executor ,
//         memory
//     );

//     std::cout << "\n===== FINAL ANSWER =====\n";

//     if (result.isSuccess())
//     {
//         std::cout << result.getData() << "\n";
//     }
//     else 
//     {
//         std::cout << "Error: " << result.getError() << "\n";
//     }

//     // std::cout << "\n\n===== Result =====" << "\n";
//     // result._print_result();

//     std::string dung;
//     std::getline(std::cin , dung);
//     std::getline(std::cin , dung);
// }

// void _run_multiple()
// {
//     // debug terminal only
//     SetConsoleOutputCP(CP_UTF8);
//     SetConsoleCP(CP_UTF8);

//     // đọc cấu hình api
//     // khởi tại ai providers
//     std::vector<maf::providers::ILLMProvider*> providers = _start_API();

//     // nhận thông tin user
//     maf::core::User user = _start_user();

//     // lấy input
//     maf::communication::Event event;
//     event._input();
// // std::cout << "===== Input =====" << "\n";
// // event._print_input();

//     // lấy input, khởi tạo quan sát, quản lý tải
//     maf::core::Observer observe;

//     const std::string input = event._return_input();

//     const maf::core::Observation observation =
//         observe.parseInput(input);

//     maf::core::Task task("001" , observation.goal);

// // std::cout << "===== OBSERVATION =====\n";
// // std::cout << "Goal: " << observation.goal << '\n';

// // for (const auto& requirement : observation.requirements)
// // {
// //     std::cout << "Requirement: " << requirement << '\n';
// // }

// // for (const auto& constraint : observation.constraints)
// // {
// //     std::cout << "Constraint: " << constraint << '\n';
// // }
// // std::cout << "===== Task =====" << "\n";
// // task._print_task();
    
//     // chuyển kiểu dữ liệu để agent dùng
//     std::vector<std::shared_ptr<maf::providers::ILLMProvider>> converted_providers = 
//         maf::agents::strategy::borrowProviders(providers);

//     // tạo context và agent
//     maf::agents::strategy::LLMStrategyContext context(
//         converted_providers ,
//         1024 ,
//         3 ,
//         4096
//     );
//     std::vector<maf::core::Agent> agents = _start_agents(providers);

//     // tạo strategy
//     maf::agents::strategy::LLMSelectionStrategy strategy(*providers[0]);

//     // tạo planner
//     maf::orchestration::Planner planner;

//     // tạo executor
//     maf::orchestration::Executor executor;

//     // tạo memory
//     maf::orchestration::Memory memory;

//     memory.setFact("observed_goal" , observation.goal);

//     const auto join_items = [](const std::vector<std::string> &items)
//     {
//         std::string joined;

//         for (const auto &item: items)
//         {
//             if (!joined.empty())
//             {
//                 joined += ", ";
//             }

//             joined += item;
//         }

//         return joined.empty() ? std::string("None") : joined;
//     };

//     memory.setFact(
//         "observed_requirements",
//         join_items(observation.requirements)
//     );

//     memory.setFact(
//         "observed_constraints",
//         join_items(observation.constraints)
//     );

//     // đăng ký tool
//     executor.registerTool(
//         std::make_shared<LLMChatTool>(context)
//     );

//     maf::core::Result result = observe._coordinate_agents(
//         agents[0] ,
//         agents[1] ,
//         task , 
//         strategy ,
//         context ,
//         planner ,
//         executor ,
//         memory
//     );

//     std::cout << "\n===== FINAL ANSWER =====\n";

//     if (result.isSuccess())
//     {
//         std::cout << result.getData() << "\n";
//     }
//     else
//     {
//         std::cout << "Error: " << result.getError() << "\n";
//     }

//     std::string dung;
//     std::getline(std::cin , dung);
//     std::getline(std::cin , dung);
// }

// int main()
// {
//     // _one_run_only();
//     _run_multiple();
// }

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

#ifdef _WIN32
#include <windows.h>
#endif

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
    const std::vector<maf::providers::ILLMProvider *> &providers
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
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

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
                                _process_request(message.toStdString(), providers)
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