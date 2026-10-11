#ifndef RESULT_H
#define RESULT_H

#include <string>
namespace maf::core {
class Result {
    private:
        bool success;
        std::string data;
        std::string Error_message;

    public:
        Result(bool isSuccess, std::string data);
        Result(bool success, std::string data, std::string error);
        bool isSuccess() const;
        std::string getData() const;
        std::string getError() const;
};

}

#endif