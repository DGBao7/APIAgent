#ifndef OBSERVE_H
#define OBSERVE_H

#include <string>
#include <vector>

namespace maf::agents {
    struct Observation {
        std::string goal;
        std::vector<std::string> requirements;
        std::vector<std::string> constraints;
    };

    class Observer
    {
    private:

    public:
        Observation parseInput(const std::string& raw_input);
        double measureSystemLoad(size_t message_queue_size, size_t active_sub_agents);
    };
} 

#endif