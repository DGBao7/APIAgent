#ifndef EVENT_H
#define EVENT_H

#include "maf/core/Task.h"

#include <iostream>
#include <string>

namespace maf::communication
{
    std::string _input();
    void _print_task(const maf::core::Task &task);
}

#endif