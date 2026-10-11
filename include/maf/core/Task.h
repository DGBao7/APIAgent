#ifndef TASK_H
#define TASK_H

#include <string>
namespace maf::core {
enum class TaskStatus {
    PENDING,
    RUNNING,
    COMPLETED,
    FAILED
};

class Task {
    private:
        std::string id;
        std::string description;
        TaskStatus status;

    public:
        Task(std::string TaskID, std::string desc);
        
        std::string getId() const;
        std::string getDesc() const;
        TaskStatus getStatus() const;
        void setStatus(TaskStatus new_status);
};

} 

#endif