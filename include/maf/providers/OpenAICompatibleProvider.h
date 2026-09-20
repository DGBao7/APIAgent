#pragma once

#include "maf/providers/ILLMProvider.h"

#include <string>
#include <vector>

namespace maf::providers
{

class OpenAICompatibleProvider : public ILLMProvider
{
protected:
    std::string provider_name;
    std::string api_key;
    std::string base_url;
    std::string model;

public:
    OpenAICompatibleProvider(
        std::string provider_name ,
        std::string api_key ,
        std::string base_url ,
        std::string model
    );

    LLMResponse _chat(
        const std::vector<Message>& messages ,
        double temperature = 0.7 ,
        int max_tokens = 1024
    ) override;

    std::string _name() const override;
};

}