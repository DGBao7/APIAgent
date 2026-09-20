#pragma once

#include "maf/providers/ILLMProvider.h"

#include <string>

namespace maf::providers
{

class GeminiProvider : public ILLMProvider
{
public:
    explicit GeminiProvider(const std::string& model = "");

    LLMResponse _chat(
        const std::vector<Message>& messages ,
        double temperature = 0.7 ,
        int max_tokens = 2048
    ) override;

    std::string _name() const override;

private:
    std::string api_key;
    std::string model;
};

}