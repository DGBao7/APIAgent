#ifndef USER_H
#define USER_H

#include <string>

namespace maf::core {

class User {
private:
    std::string user_id;
    std::string user_name;

public:
    User(std::string id, std::string name);

    std::string getUserId() const;
    std::string getUserName() const;
};

} 

#endif