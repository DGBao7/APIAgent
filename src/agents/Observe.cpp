#include "maf/agents/observe.h"
#include <sstream>
#include <algorithm>

namespace maf::agents {
    Observation Observer::parseInput(const std::string& raw_input) 
    {
        Observation obs;
        
        obs.goal = raw_input; 
        
        obs.requirements.push_back("Standard execution");
        obs.constraints.push_back("None");

        return obs;
    }

    double Observer::measureSystemLoad(size_t message_queue_size, size_t active_sub_agents) 
    {
        double load_score = (message_queue_size * 1.5) + (active_sub_agents * 0.5);

        return load_score;
    }
}
