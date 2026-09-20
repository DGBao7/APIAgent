#pragma once

#include "maf/providers/OpenAICompatibleProvider.h"

namespace maf::providers
{

class OpenRouterProvider : public OpenAICompatibleProvider
{
public:
    explicit OpenRouterProvider(const std::string& model = "");
};

}
