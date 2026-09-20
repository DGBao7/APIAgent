#include "maf/utils/Env.h"

#include <cstdlib>
#include <fstream>
#include <string>

namespace
{
std::string _trim(const std::string& value)
{
    const std::string whitespace = " \t\r\n";

    const std::size_t first = value.find_first_not_of(whitespace);

    if (first == std::string::npos)
    {
        return "";
    }

    const std::size_t last = value.find_last_not_of(whitespace);

    return value.substr(first , last - first + 1);
}

std::string _remove_quotes(const std::string& value)
{
    if (value.size() >= 2)
    {
        const char first = value.front();
        const char last = value.back();

        if ((first == '"' && last == '"') || (first == '\'' && last == '\''))
        {
            return value.substr(1 , value.size() - 2);
        }
    }

    return value;
}
}

namespace maf::utils
{
bool Env::_load(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        return false;
    }

    std::string line;

    while (std::getline(file , line))
    {
        line = _trim(line);

        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        if (line.rfind("export " , 0) == 0)
        {
            line = _trim(line.substr(7));
        }

        const std::size_t separator = line.find('=');

        if (separator == std::string::npos)
        {
            continue;
        }

        const std::string key = _trim(
            line.substr(0 , separator)
        );

        const std::string value = _remove_quotes(
            _trim(line.substr(separator + 1))
        );

        if (key.empty())
        {
            continue;
        }

#ifdef _WIN32

        _putenv_s(key.c_str() , value.c_str());

#else

        setenv(key.c_str() , value.c_str() , 1);

#endif
    }

    return true;
}

std::string Env::_get(const std::string& key)
{
    const char* value = std::getenv(key.c_str());

    if (value == nullptr)
    {
        return "";
    }

    return value;
}
}