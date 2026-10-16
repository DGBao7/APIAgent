#ifndef TASK_H
#define TASK_H

#include <string>
#include <iostream>

namespace maf::core {
    enum class TaskStatus 
    {
        PENDING,
        RUNNING,
        COMPLETED,
        FAILED
    };

    class Task 
    {
    private:
        std::string id;
        std::string description;
        TaskStatus status;

    public:
        Task(std::string TaskID , std::string input);
        
        std::string getId() const;
        std::string getDesc() const;
        TaskStatus getStatus() const;

        void setStatus(TaskStatus new_status);

        static Task _return_task(std::string input);
        void _print_task();
    };
} 

#endif