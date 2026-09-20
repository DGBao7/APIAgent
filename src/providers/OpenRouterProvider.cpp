#include "maf/providers/OpenRouterProvider.h"
#include "maf/utils/Env.h"

namespace maf::providers
{

OpenRouterProvider::OpenRouterProvider(const std::string& selected_model)
    : OpenAICompatibleProvider(
        "OpenRouter" ,
        maf::utils::Env::_get("OPENROUTER_API_KEY") ,
        "https://openrouter.ai/api/v1" ,
        selected_model.empty()
            ? (maf::utils::Env::_get("OPENROUTER_MODEL").empty()
                ? "openrouter/free"
                : maf::utils::Env::_get("OPENROUTER_MODEL"))
            : selected_model
    )
{
}

}