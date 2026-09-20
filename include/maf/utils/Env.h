#pragma once

#include <string>

namespace maf::utils
{
class Env
{
public:
    static bool _load(const std::string& path);

    static std::string _get(const std::string& key);
};
}