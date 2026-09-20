#include "maf/providers/HttpClient.h"

#include <curl/curl.h>
#include <mutex>
#include <string>

namespace
{

size_t _write_callback(
    char* data ,
    size_t size ,
    size_t count ,
    void* user_data
)
{
    const std::size_t total = size * count;

    auto* output = static_cast<std::string*>(user_data);

    output -> append(data , total);

    return total;
}

void _init_curl()
{
    static std::once_flag flag;

    std::call_once(flag , []()
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    });
}

}

namespace maf::providers
{

HttpResponse HttpClient::_post(
    const std::string& url ,
    const std::vector<std::string>& headers ,
    const std::string& body
) const
{
    _init_curl();

    HttpResponse response;

    CURL* curl = curl_easy_init();

    if (curl == nullptr)
    {
        response.error = "curl_easy_init failed";

        return response;
    }

    struct curl_slist* header_list = nullptr;

    for (const std::string& header : headers)
    {
        header_list = curl_slist_append(
            header_list ,
            header.c_str()
        );
    }

    curl_easy_setopt(
        curl ,
        CURLOPT_URL ,
        url.c_str()
    );

    curl_easy_setopt(
        curl ,
        CURLOPT_HTTPHEADER ,
        header_list
    );

    curl_easy_setopt(
        curl ,
        CURLOPT_POST ,
        1L
    );

    curl_easy_setopt(
        curl ,
        CURLOPT_POSTFIELDS ,
        body.c_str()
    );

    curl_easy_setopt(
        curl ,
        CURLOPT_POSTFIELDSIZE ,
        static_cast<long>(body.size())
    );

    curl_easy_setopt(
        curl ,
        CURLOPT_WRITEFUNCTION ,
        _write_callback
    );

    curl_easy_setopt(
        curl ,
        CURLOPT_WRITEDATA ,
        &response.body
    );

    curl_easy_setopt(
        curl ,
        CURLOPT_TIMEOUT ,
        120L
    );

    curl_easy_setopt(
        curl ,
        CURLOPT_CONNECTTIMEOUT ,
        20L
    );

    const CURLcode result = curl_easy_perform(curl);

    if (result != CURLE_OK)
    {
        response.error = curl_easy_strerror(result);
    }

    curl_easy_getinfo(
        curl ,
        CURLINFO_RESPONSE_CODE ,
        &response.status_code
    );

    curl_slist_free_all(header_list);

    curl_easy_cleanup(curl);

    return response;
}

}