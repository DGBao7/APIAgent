#include "maf/agents/Strategy.h"

namespace maf::agents::strategy {

LLMStrategyContext::LLMStrategyContext(std::shared_ptr<maf::providers::ILLMProvider> provider)
    : provider(std::move(provider)) {}

std::shared_ptr<maf::providers::ILLMProvider> LLMStrategyContext::getProvider() const {
    return provider;
}

} 