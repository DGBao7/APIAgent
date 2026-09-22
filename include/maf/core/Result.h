#ifndef RESULT_H
#define RESULT_H
#include <string>
using namespace std;
class Result
{
    private:
        bool success;
        string data;
        string Error_message;
    public:
        Result(bool isSuccess, string data)
        :success(success), data(data) {}
        Result(bool success, string data, string error)
        :success(success), data(data), Error_message(error) {}

        bool isSuccess() const {return success;}
        string getData() const {return data;}
        string getError() const {return Error_message;}
};
#endif