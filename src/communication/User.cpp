#include "maf/communication/User.h"

class User;

void User::_take_input()
{
    std::string input;

    std::cout << "Question: ";
    std::getline(std::cin , input);

    this -> input = input;
}

std::string User::_return_input()
{
    return this -> input;
}