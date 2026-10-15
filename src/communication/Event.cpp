#include "maf/communication/Event.h"

namespace maf::communication
{
    std::string _input()
    {
        std::string input;
        std::cout << "Question: ";
        std::cin.ignore();
        std::getline(std::cin , input);

        return input;
    }

    void _print_task(const maf::core::Task &task)
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