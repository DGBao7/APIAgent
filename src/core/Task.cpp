#include "maf/core/Task.h"

namespace maf::core 
{
    Task::Task(std::string TaskID , std::string input)
    {
        this -> id = TaskID;
        this -> description = input;
        this -> status = TaskStatus::PENDING;
    }

    std::string Task::getId() const 
    {
        return id;
    }

    std::string Task::getDesc() const 
    {
        return description;
    }

    TaskStatus Task::getStatus() const 
    {
        return status;
    }

    void Task::setStatus(TaskStatus new_status) 
    {
        status = new_status;
    }

    Task Task::_return_task(std::string input)
    {
        return Task("task1" , input);
    }

    void Task::_print_task()
    {
        std::cout << "Task ID: " << this -> getId() << '\n';
        std::cout << "Description: " << this -> getDesc() << '\n';
        std::cout << "Status: " << static_cast<int>(this -> getStatus()) << '\n';
    }
}