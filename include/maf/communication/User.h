#ifndef USER_H
#define USER_H

#include <string>
#include <iostream>

class User
{
private:
    std::string input;

public:
    void _take_input();
    std::string _return_input();
};

#endif