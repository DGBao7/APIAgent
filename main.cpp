#include "maf/utils/Env.h"

#include "maf/providers/GeminiProvider.h"
#include "maf/providers/GroqProvider.h"
#include "maf/providers/OpenRouterProvider.h"
#include "maf/providers/ILLMProvider.h"

#include "maf/communication/Event.h"

#include "maf/core/Task.h"
#include "maf/core/Agent.h"
#include "maf/core/User.h"
#include "maf/core/Result.h"

#include "maf/agents/Strategy.h"

#include "maf/orchestration/Planner.h"
#include "maf/orchestration/Executor.h"
#include "maf/orchestration/Memory.h"

#include <iostream>
#include <vector>
#include <filesystem>
#include <string>
#include <memory>
#include <stdexcept>
#include <windows.h>

std::vector<maf::providers::ILLMProvider*> _start_API()
{
    const bool loaded = maf::utils::Env::_load(".env");

    if (!loaded)
    {
        std::cerr << 
            "Can not load .env from: "
            << std::filesystem::current_path()
            << "\n";
    }
    else
    {
        std::cerr << 
            "Env loaded successfully" << "\n";
    }

    maf::utils::Env::_load(".env"); // Doc .env

    std::vector<maf::providers::Message> messages = {
        {"system" , "You are a concise AI agent."} ,
        {"user" , "Hello. Explain what an agent is in one sentence."}
    };

    static maf::providers::GeminiProvider gemini;
    static maf::providers::GroqProvider groq;
    static maf::providers::OpenRouterProvider open_router;

    std::vector<maf::providers::ILLMProvider*> providers = {
        &gemini ,
        &groq ,
        &open_router
    };

    for (maf::providers::ILLMProvider* provider : providers)
    {
        const auto response = provider -> _chat(messages);

        std::cout << "[" << provider -> _name() << "]\n";

        if (response.ok)
        {
            std::cout << response.text << "\n\n";
        }
        else
        {
            std::cout << "Error: " << response.error << "\n\n";
        }
    }

    return providers;
}

maf::core::User _start_user()
{
    return maf::core::User("123" , "Bon");
}

std::vector<maf::core::Agent> _start_agents(
    std::vector<maf::providers::ILLMProvider*> providers
)
{
    std::vector<maf::core::Agent> agents;

    for (int i = 0; i < providers.size(); i ++)
    {
        agents.push_back(maf::core::Agent(
            std::to_string(i) ,
            "Bon" + std::to_string(i) ,
            providers[i]
        ));
    }

    return agents;
}

// lấy provider được chọn, gửi yêu cầu đến AI, trả giá trị về executor
// t viết vào đây vì trông orches sợ quá deo dám động vào
class LLMChatTool : public maf::orchestration::ITool
{
private:
    maf::agents::strategy::LLMStrategyContext *context;
public:
    explicit LLMChatTool(
        maf::agents::strategy::LLMStrategyContext &context
    )
    {
        this -> context = &context;
    }

    std::string getName() const override
    {
        return "llm chat tool";
    }

    std::string execute(const std::string &input) override
    {
        std::vector<maf::providers::Message> messages = {
            {
                "system" ,
                "You are my assistant. Be who you are and give me the best"
            } ,
            {"user" , input}
        };

        const auto provider_count = this -> context -> getProviderNames().size();
        std::string last_error;

        for (size_t i = 0; i < provider_count; i ++)
        {
            const auto provider = this -> context -> getProvider();

            if (!provider)
            {
                break;
            }

std::cout << "[DEBUG] Provider: "
        << provider -> _name() << "\n";

            const auto response = provider -> _chat(
                messages ,
                0.2 ,
                context -> getMaxTokens()
            );

std::cout << "[DEBUG] Provider: "
        << provider -> _name() << "\n";
std::cout << "[DEBUG] Response: "
          << response.text << "\n";

            if (response.ok)
            {
                return response.text;
            }

            last_error = response.error;

            if (i + 1 < provider_count)
            {
                context -> switchToNextProvider();
            }
        }

        throw std::runtime_error(
            "All LLM providers failed. Last error: " + last_error
        );
    }
};

int main()
{
    // debug terminal only
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // đọc cấu hình api
    // khởi tại ai providers
    std::vector<maf::providers::ILLMProvider*> providers = _start_API();

    // nhận thông tin user
    maf::core::User user = _start_user();

    // lấy input
    maf::communication::Event event;
    event._input();
// std::cout << "===== Input =====" << "\n";
// event._print_input();

    // biểu diễn yêu cầu
    maf::core::Task task("001" , event._return_input());
// std::cout << "===== Task =====" << "\n";
// task._print_task();
    
    // chuyển kiểu dữ liệu để agent dùng
    std::vector<std::shared_ptr<maf::providers::ILLMProvider>> converted_providers = 
        maf::agents::strategy::borrowProviders(providers);

    // tạo context và agent
    maf::agents::strategy::LLMStrategyContext context(
        converted_providers ,
        1024 ,
        3 ,
        4096
    );
    maf::core::Agent agent("2006" , "Bon" , providers[1]);

    // tạo strategy
    maf::agents::strategy::LLMSelectionStrategy strategy(*providers[1]);

    // tạo planner
    maf::orchestration::Planner planner;

    // tạo executor
    maf::orchestration::Executor executor;

    // tạo memory
    maf::orchestration::Memory memory;

    // đăng ký tool
    executor.registerTool(
        std::make_shared<LLMChatTool>(context)
    );

    maf::core::Result result = agent._workflow(
        task ,
        strategy ,
        context ,
        planner ,
        executor ,
        memory
    );

    std::cout << "\n===== FINAL ANSWER =====\n";

    if (result.isSuccess())
    {
        std::cout << result.getData() << "\n";
    }
    else 
    {
        std::cout << "Error: " << result.getError() << "\n";
    }

    // std::cout << "\n\n===== Result =====" << "\n";
    // result._print_result();

    std::string unclose;
    std::getline(std::cin , unclose);
    std::getline(std::cin , unclose);
}