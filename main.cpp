#include "maf/utils/Env.h"

#include "maf/providers/GeminiProvider.h"
#include "maf/providers/GroqProvider.h"
#include "maf/providers/OpenRouterProvider.h"

#include "maf/communication/Event.h"

#include "maf/core/Task.h"
#include "maf/core/Agent.h"
#include "maf/core/User.h"

#include <iostream>
#include <vector>
#include <filesystem>
#include <string>

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

int main()
{
    // đọc cấu hình api
    // khởi tại ai providers
    std::vector<maf::providers::ILLMProvider*> providers = _start_API();

    // nhận thông tin user
    maf::core::User user = _start_user();

    // lấy input
    maf::communication::Event event;
    event._input();
    std::cout << "===== Input =====" << "\n";
    event._print_input();

    // biểu diễn yêu cầu
    maf::core::Task task("001" , event._return_input());
    task._print_task();
    
    // xử lý task   
    std::vector<maf::core::Agent> agents = _start_agents(providers);
    for (auto agent : agents)
    {
        agent._workflow(task);
    }
}