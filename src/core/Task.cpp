#include "maf/core/Task.h"

namespace maf::core {

Task::Task(std::string TaskID, std::string desc)
    :id(TaskID), description(desc), status(TaskStatus::PENDING) {}

std::string Task::getId() const {return id;}
std::string Task::getDesc() const {return description;}
TaskStatus Task::getStatus() const {return status;}
void Task::setStatus(TaskStatus new_status) {status = new_status;}

} 