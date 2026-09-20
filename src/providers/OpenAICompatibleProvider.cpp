// Xu ly cac chung cac api tuong thich openai

#include "maf/providers/OpenAICompatibleProvider.h"
#include "maf/providers/HttpClient.h"

#include <nlohmann/json.hpp>
#include <utility>

// Dung json de request nhu sau
// {
//     "model": "...",
//     "messages": [
//         {
//             "role": "system",
//             "content": "You are an AI."
//         },
//         {
//             "role": "user",
//             "content": "Hello"
//         }
//     ],
//     "temperature": 0.7,
//     "max_tokens": 1024
// }

namespace
{
using json = nlohmann::json;
}
// constructor
namespace maf::providers
{

OpenAICompatibleProvider::OpenAICompatibleProvider(
    std::string provider_name ,
    std::string api_key ,
    std::string base_url ,
    std::string model
)
    : provider_name(std::move(provider_name)) ,
      api_key(std::move(api_key)) ,
      base_url(std::move(base_url)) ,
      model(std::move(model))
{
}

LLMResponse OpenAICompatibleProvider::_chat(
    const std::vector<Message>& messages ,
    double temperature ,
    int max_tokens
)
{
    LLMResponse result;

    result.provider = provider_name;
    result.model = model;

    if (api_key.empty())
    {
        result.error = provider_name + " API key is empty";

        return result;
    }

    json body;

    body["model"] = model;
    body["messages"] = json::array();
    body["temperature"] = temperature;
    body["max_tokens"] = max_tokens;

    for (const Message& message : messages)
    {
        body["messages"].push_back({
            {"role" , message.role},
            {"content" , message.content}
        });
    }

    HttpClient client;

    const HttpResponse response = client._post(
        base_url + "/chat/completions" ,
        {
            "Content-Type: application/json" ,
            "Authorization: Bearer " + api_key
        } ,
        body.dump()
    );

    result.status_code = static_cast<int>(response.status_code);
    result.raw = response.body;

    if (!response.error.empty())
    {
        result.error = response.error;

        return result;
    }

    json data;

    try
    {
        data = json::parse(response.body);
    }
    catch (const std::exception& error)
    {
        result.error =
            std::string("Invalid JSON response: ") +
            error.what();

        return result;
    }

    if (response.status_code < 200 ||
        response.status_code >= 300)
    {
        if (data.contains("error"))
        {
            if (data["error"].is_object() &&
                data["error"].contains("message"))
            {
                result.error =
                    data["error"]["message"].get<std::string>();
            }
            else
            {
                result.error = data["error"].dump();
            }
        }
        else
        {
            result.error = response.body;
        }

        return result;
    }

    if (!data.contains("choices") ||
        !data["choices"].is_array() ||
        data["choices"].empty())
    {
        result.error = "Response does not contain choices";

        return result;
    }

    const json& choice = data["choices"][0];

    if (!choice.contains("message") ||
        !choice["message"].contains("content"))
    {
        result.error = "Response does not contain message.content";

        return result;
    }

    result.text = choice["message"]["content"].get<std::string>();
    result.ok = true;

    return result;
}

std::string OpenAICompatibleProvider::_name() const
{
    return provider_name;
}
}