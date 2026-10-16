#include "maf/core/User.h"

namespace maf::core 
{
    User::User(std::string id, std::string name)
    {
        this -> user_id = id;
        this -> user_name = user_name;
    }

    std::string User::getUserId() const 
    {
        return user_id;
    }

    std::string User::getUserName() const 
    {
        return user_name;
    }
}
