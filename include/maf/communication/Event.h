#ifndef EVENT_H
#define EVENT_H

#include "maf/core/Task.h"

#include <iostream>
#include <string>

namespace maf::communication
{
    class Event
    {
    private:
        std::string input;

    public:
        void _input();
        void _print_input();
        void _print_task(const maf::core::Task &task);

        std::string _return_input();
    };
}

#endif