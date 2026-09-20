#include "maf/providers/GeminiProvider.h"

#include "maf/providers/HttpClient.h"
#include "maf/utils/Env.h"

#include <nlohmann/json.hpp>

#include <utility>

namespace
{

using json = nlohmann::json;

}

namespace maf::providers
{

GeminiProvider::GeminiProvider(const std::string& selected_model)
    : api_key(maf::utils::Env::_get("GEMINI_API_KEY")) ,
      model(
          selected_model.empty()
              ? (
                  maf::utils::Env::_get("GEMINI_MODEL").empty()
                      ? "gemini-2.5-flash"
                      : maf::utils::Env::_get("GEMINI_MODEL")
                )
              : selected_model
        )
{
}

LLMResponse GeminiProvider::_chat(
    const std::vector<Message>& messages ,
    double temperature ,
    int max_tokens
)
{
    LLMResponse result;

    result.provider = _name();
    result.model = model;

    if (api_key.empty())
    {
        result.error = "Gemini API key is empty";

        return result;
    }

    json body;

    body["contents"] = json::array();

    for (const Message& message : messages)
    {
        if (message.role == "system")
        {
            if (!body.contains("systemInstruction"))
            {
                body["systemInstruction"] = {
                    {"parts" , json::array()}
                };
            }

            body["systemInstruction"]["parts"].push_back({
                {"text" , message.content}
            });

            continue;
        }

        const std::string role =
            message.role == "assistant" ? "model" : "user";

        body["contents"].push_back({
            {"role" , role},
            {"parts" , {
                {"text" , message.content}
            }}
        });
    }

    body["generationConfig"] = {
        {"temperature" , temperature},
        {"maxOutputTokens" , max_tokens}
    };

    const std::string url =
        "https://generativelanguage.googleapis.com/v1beta/models/" +
        model +
        ":generateContent";

    HttpClient client;

    const HttpResponse response = client._post(
        url ,
        {
            "Content-Type: application/json" ,
            "x-goog-api-key: " + api_key
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
        if (data.contains("error") &&
            data["error"].is_object())
        {
            if (data["error"].contains("message"))
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

    if (!data.contains("candidates") ||
        !data["candidates"].is_array() ||
        data["candidates"].empty())
    {
        result.error =
            "Gemini response does not contain candidates";

        return result;
    }

    const json& candidate = data["candidates"][0];

    if (!candidate.contains("content") ||
        !candidate["content"].contains("parts"))
    {
        result.error =
            "Gemini response does not contain content.parts";

        return result;
    }

    std::string text;

    for (const json& part : candidate["content"]["parts"])
    {
        if (part.contains("text") &&
            part["text"].is_string())
        {
            text += part["text"].get<std::string>();
        }
    }

    if (text.empty())
    {
        result.error =
            "Gemini returned an empty text response";

        return result;
    }

    result.text = std::move(text);
    result.ok = true;

    return result;
}

std::string GeminiProvider::_name() const
{
    return "Gemini";
}

}