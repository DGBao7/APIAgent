#include "maf/core/Result.h"

namespace maf::core {
    Result::Result(bool isSuccess, std::string data)
        : success(isSuccess), data(data), Error_message("") {}

    Result::Result(bool success, std::string data, std::string error)
        : success(success), data(data), Error_message(error) {}

    bool Result::isSuccess() const {
        return success;
    }

    std::string Result::getData() const {
        return data;
    }

    std::string Result::getError() const {
        return Error_message;
    }
    
    void Result::_print_result()
    {
        std::cout << this -> success << "\n";
        std::cout << "Data: " << this -> data << "\n";
        std::cout << "Error: " << this -> Error_message << "\n";
    }
} 