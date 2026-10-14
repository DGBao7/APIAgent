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

} 

#endif