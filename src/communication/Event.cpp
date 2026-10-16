#include "maf/communication/Event.h"

namespace maf::communication
{
    void Event::_input()
    {
        std::cout << "Question: ";
        std::cin.ignore();
        std::getline(std::cin , this -> input);
    }

    void Event::_print_input()
    {
        std::cout << "Input: " << this -> input << "\n";
    }

    std::string Event::_return_input()
    {
        return this -> input;
    }

    void Event::_print_task(const maf::core::Task &task)
    {
        std::cout << "\n===== TASK =====\n";
        std::cout << "ID: " << task.getId() << '\n';
        std::cout << "Description: " << task.getDesc() << '\n';
        std::cout << "Status: ";

        switch (task.getStatus())
        {
            case maf::core::TaskStatus::PENDING:
                std::cout << "PENDING";
                break;

            case maf::core::TaskStatus::RUNNING:
                std::cout << "RUNNING";
                break;

            case maf::core::TaskStatus::COMPLETED:
                std::cout << "COMPLETED";
                break;

            case maf::core::TaskStatus::FAILED:
                std::cout << "FAILED";
                break;
        }

        std::cout << '\n';
    }
}