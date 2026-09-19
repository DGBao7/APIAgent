#ifndef TASK_H
#define TASK_H
#include <string>
using namespace std;
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
        string id;
        string description;
        TaskStatus status;
    public:
        Task(string TaskID, string desc)
        :id(TaskID), description(desc), status(TaskStatus::PENDING){}
        string getId() const {return id;}
        string getDesc() const {return description;}
        TaskStatus getStatus() const {return status;}

        void setStatus(TaskStatus new_status)
        {   
            status = new_status;
        }
};
#endif