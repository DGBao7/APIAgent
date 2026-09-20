#include "maf/utils/Env.h"
#include "maf/providers/GeminiProvider.h"
#include "maf/providers/GroqProvider.h"
#include "maf/providers/OpenRouterProvider.h"

#include <iostream>
#include <vector>

int main()
{
    maf::utils::Env::_load(".env");

    std::vector<maf::providers::Message> messages = {
        {"system" , "You are a concise AI agent."} ,
        {"user" , "Hello. Explain what an agent is in one sentence."}
    };

    maf::providers::GeminiProvider gemini;
    maf::providers::GroqProvider groq;
    maf::providers::OpenRouterProvider open_router;

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

    return 0;
}