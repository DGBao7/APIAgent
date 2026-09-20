#include "maf/providers/GroqProvider.h"

#include "maf/utils/Env.h"

namespace maf::providers
{

GroqProvider::GroqProvider(const std::string& selected_model)
    : OpenAICompatibleProvider(
        "Groq" ,
        maf::utils::Env::_get("GROQ_API_KEY") ,
        "https://api.groq.com/openai/v1" ,
        selected_model.empty()
            ? (maf::utils::Env::_get("GROQ_MODEL").empty()
                ? "openai/gpt-oss-20b"
                : maf::utils::Env::_get("GROQ_MODEL"))
            : selected_model
    )
{
}

}