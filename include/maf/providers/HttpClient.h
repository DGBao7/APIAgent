#pragma once

#include <string>
#include <vector>

namespace maf::providers
{

struct HttpResponse
{
    std::string body;
    std::string error;
    long status_code = 0;
};

class HttpClient
{
public:
    HttpResponse _post(
        const std::string& url ,
        const std::vector<std::string>& headers ,
        const std::string& body
    ) const;
};

}