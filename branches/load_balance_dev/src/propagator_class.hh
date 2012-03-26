/*
 * propagator_class.hh
 *
 *  Created on: Feb 24, 2010
 *      Author: jentsch
 */

#ifndef PROPAGATOR_CLASS_HH_
#define PROPAGATOR_CLASS_HH_

#include "agent/agent.hh"
#include "composer/composer.hh"

#include <map>

using namespace std;

namespace Hive {

class PropagatorClass {
  public:
    PropagatorClass():agentsChanged_(false) {}
    ~PropagatorClass() {}

    void   setup(Composer *composer);
    void   go(double dt, double endTime);
    double getCurrentTime() { return currentTime_; }

    void   removeAgent(int id);
    void   addAgent(Agent* agent);
    void   updateAgents();
    Agent* getAgent(int agentId);

    map<int, Agent*>* getAgentMapPointer() { return &agentMap_; }

  protected:
    // vector of agents that live on this node
    vector<Agent*> agents_;     // obsolete
    // composer that was used to set up simulation on this node
    Composer *composer_;
    // the communicator that knows how to send and get messages
    Communicator* communicator_;
    // the internal clock on this node
    double currentTime_;
    // id to agent
    map<int, Agent*> agentMap_;
    // flag 
    bool agentsChanged_;

};

}

#endif /* PROPAGATOR_CLASS_HH_ */
