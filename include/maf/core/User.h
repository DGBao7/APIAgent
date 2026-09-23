#ifndef USER_H
#define USER_H
#include <string>
using namespace std;
class User
{
    private:
        string user_id;
        string user_name;
    public:
        User(string id, string name)
        :user_id(id), user_name(name){}
        string getUserId() const {return user_id;}
        string getUserName() const {return user_name;}
};
#endif