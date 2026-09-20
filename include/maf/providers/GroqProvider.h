#pragma once

#include "maf/providers/OpenAICompatibleProvider.h"

namespace maf::providers
{

class GroqProvider : public OpenAICompatibleProvider
{
public:
    explicit GroqProvider(const std::string& model = "");
};

}