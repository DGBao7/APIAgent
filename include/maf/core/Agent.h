#ifndef AGENT_H
#define AGENT_H
#include <string>
#include "Result.h"
#include "Task.h"
using namespace std;
class Agent
{
    private:
        string agentId;
        string agentName;
    public:
        Agent(string id, string name)
        :agentId(id), agentName(name){}
        virtual ~Agent();
        string getId() const {return agentId;}
        string getName() const {return agentName;}
        virtual Result execute(Task& task) = 0;
};
#endif