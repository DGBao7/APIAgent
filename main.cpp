#include "maf/utils/Env.h"
#include "maf/providers/GeminiProvider.h"
#include "maf/providers/GroqProvider.h"
#include "maf/providers/OpenRouterProvider.h"
#include "maf/communication/User.h"

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

maf::providers::ILLMProvider* _choose_AI(std::vector<maf::providers::ILLMProvider*> providers)
{
    return providers[1];
}

void _communication(User user , std::vector<maf::providers::ILLMProvider*> providers)
{
    maf::providers::ILLMProvider* AI_selected;

    while (true)
    {
        user._take_input();

        if (user._return_input() == "exits")
        {
            break;
        }

        std::vector<maf::providers::Message> messages = {
            {"system" , "You are a concise AI agent."} ,
            {"user" , user._return_input()}
        };

        AI_selected = _choose_AI(providers);

        const auto response = AI_selected -> _chat(messages);

        if (response.ok)
        {
            std::cout << AI_selected -> _name() << ": "
                    << response.text << "\n";
        }
        else 
        {
            std::cout << "[Error]" << response.error << "\n";
        }
    }
}

int main()
{
    std::vector<maf::providers::ILLMProvider*> providers = _start_API();

    User user = _start_user();

    _communication(user , providers);
}