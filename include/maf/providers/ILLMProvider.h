#pragma once

#include <string>
#include <vector>

namespace maf::providers
{

struct Message
{
    std::string role;
    std::string content;
};

struct LLMResponse
{
    bool ok = false;

    std::string provider;
    std::string model;
    std::string text;
    std::string error;
    std::string raw;

    int status_code = 0;
};

class ILLMProvider
{
public:
    virtual ~ILLMProvider();

    virtual LLMResponse _chat(
        const std::vector<Message>& messages ,
        double temperature = 0.7 ,
        int max_tokens = 1024
    ) = 0;

    virtual std::string _name() const = 0;
};

}