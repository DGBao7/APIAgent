#include "maf/utils/Env.h"
#include "maf/providers/GeminiProvider.h"
#include "maf/providers/GroqProvider.h"
#include "maf/providers/OpenRouterProvider.h"
#include "maf/communication/User.h"
#include "maf/core/Task.h"
#include "maf/communication/Event.h"

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

User _start_user()
{
    User user;

    return user;
}

int main()
{
    std::vector<maf::providers::ILLMProvider*> providers = _start_API();

    User user = _start_user();

    std::string input = maf::communication::_input();

    maf::core::Task task = maf::core::Task::_return_task(input);
    task._print_task();
}