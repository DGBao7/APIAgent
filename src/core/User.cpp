#include "maf/core/User.h"

namespace maf::core {

User::User(std::string id, std::string name)
    :user_id(id), user_name(name) {}

std::string User::getUserId() const {return user_id;}
std::string User::getUserName() const {return user_name;}

} 