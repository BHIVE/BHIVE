#include <stdlib.h>

#include "propagator_class.hh"
#include "parallel/parallel.hh"
#include "hive.hh"

using namespace Hive;

void PropagatorClass::setup(Composer *composer) {
    set_propagator(this);

	composer_      = composer;
	communicator_  = composer->getCommunicator();
	currentTime_   = 0;        

    cerr << "Number of agents is: " << composer->getNumberAgents() << endl;
	for (int i = 0; i < composer->getNumberAgents(); ++i){
        Agent* agent = composer->getAgent(i);
        agent->setPropagator(this);
        addAgent(agent);
	}
    updateAgents();
    composer->setupSimulation();
}

void PropagatorClass::go(double dt, double endTime) {
	// Here is the main simulation loop
	for (; currentTime_ < endTime; currentTime_ += dt){
        cerr << "Simulating " << currentTime_ << " of " << endTime << endl;
		map<int, Agent*>::iterator it;
		// STEPS 1-3: PRE MESSAGE SENDING, RECIEVING, AND EVALUATION
		// STEP  4:   RUN SIMULATORS
		// STEPS 6-8: POST MESSAGE SENDING, RECIEVING, AND EVALUATION

		// STEP 1:  tell each agent to place its messages in its outbox,
		// then tell each agent to purge its outbox by sending the messages
		// the communicator will immediately send all messages that are to
		// be delivered within the same node, and collect all messages to
		// be delivered to other nodes into a nice little packag
        
        for (it = agentMap_.begin(); it != agentMap_.end(); ++it) {
			it->second->placePreMessagesInOutbox();
			it->second->sendMessages();
            // cout << mpirank() << " it->second->getAgentNode()  = " << it->second->getAgentNode() << endl;
            // cout << mpirank() << " it->second->getParentNode() = " << it->second->getParentNode() << endl;
            
		}
		// STEP 2:  the communicator will now send messages
		communicator_->parallelCommunicate();

		// STEP 3:  agents handle any messages they received
        for (it = agentMap_.begin(); it != agentMap_.end(); ++it) {
			it->second->evaluateMessageQueue();
		}        
		// remove old messages from communicators message queue ...
		communicator_->clearQueues();

		// STEP 4:  agents are allowed to start their simulators
		// note: you could do this in multiple threads!
        for (it = agentMap_.begin(); it != agentMap_.end(); ++it) {
            // fprintf(stderr, "~~~~~ before advanceSimulators, agentId = %d \n", it->second->getAgentId());
            it->second->advanceSimulators(dt);
            // cerr << "~~~~~ after advanceSimulators ~~~~~~~~~" << endl;
		}
		// STEP 5:  tell each agent to place its messages in its outbox,
		// then tell each agent to purge its outbox by sending the messages
		// the communicator will immediately send all messages that are to
		// be delivered within the same node, and collect all messages to
		// be delivered to other nodes into a nice little packag
        // cout << "--------" << endl;
        for (it = agentMap_.begin(); it != agentMap_.end(); ++it) {
            // cout << "[" << mpirank() << "] it->first = " << it->first << endl;
			it->second->placePostMessagesInOutbox();
			it->second->sendMessages();
		}
		// STEP 6:  the communicator will now send messages
		communicator_->parallelCommunicate();
		// STEP 7:  agents handle any messages they received
        for (it = agentMap_.begin(); it != agentMap_.end(); ++it) {
			it->second->evaluateMessageQueue();
		}
		communicator_->clearQueues();
        updateAgents();         // not necessary: only to keep orchestra up-to-date

	}
}

/*
void PropagatorClass::go_old(double dt, double endTime) {
	// Here is the main simulation loop
	for (; currentTime < endTime; currentTime += dt){
		// STEPS 1-3: PRE MESSAGE SENDING, RECIEVING, AND EVALUATION
		// STEP  4:   RUN SIMULATORS
		// STEPS 6-8: POST MESSAGE SENDING, RECIEVING, AND EVALUATION

		// STEP 1:  tell each agent to place its messages in its outbox,
		// then tell each agent to purge its outbox by sending the messages
		// the communicator will immediately send all messages that are to
		// be delivered within the same node, and collect all messages to
		// be delivered to other nodes into a nice little packag
		for(unsigned int a=0; a<this->agents.size(); a++) {
			agents[a]->placePreMessagesInOutbox();
			agents[a]->sendMessages();
		}
		// STEP 2:  the communicator will now send messages
		this->communicator->parallelCommunicate();
		// STEP 3:  agents handle any messages they received
		for(unsigned int a=0; a<this->agents.size(); a++) {
			agents[a]->evaluateMessageQueue();
		}        
		// remove old messages from communicators message queue ...
		this->communicator->clearQueues();
        updateAgents();

		// STEP 4:  agents are allowed to start their simulators
		// note: you could do this in multiple threads!
		for(unsigned int a=0; a<this->agents.size(); a++) {
			agents[a]->advanceSimulators(dt);
		}
        updateAgents();
		// STEP 5:  tell each agent to place its messages in its outbox,
		// then tell each agent to purge its outbox by sending the messages
		// the communicator will immediately send all messages that are to
		// be delivered within the same node, and collect all messages to
		// be delivered to other nodes into a nice little packag
		for(unsigned int a=0; a<this->agents.size(); a++) {
			agents[a]->placePostMessagesInOutbox();
			agents[a]->sendMessages();
		}
        updateAgents();

		// STEP 6:  the communicator will now send messages
		this->communicator->parallelCommunicate();
		// STEP 7:  agents handle any messages they received
		for(unsigned int a=0; a<this->agents.size(); a++) {
			agents[a]->evaluateMessageQueue();
		}
		this->communicator->clearQueues();
        updateAgents();

	}
}
*/

void PropagatorClass::removeAgent(int id) {
    map<int, Agent*>::iterator it = agentMap_.find(id);
    cout << "removing agent id = " << id << endl;
    
    if (it == agentMap_.end()) {
        cerr << "Error: attempting to delete a nonexistent agent " << id << endl;
        return;
    } 
    // cout << "before agentMap erase, size = " << agentMap_.size() << endl;
    Agent* agent = it->second;
    // agentMap_.erase(it);
    agentMap_.erase(id);
    // cout << "after  agentMap erase, size = " << agentMap_.size() << endl;
    // cout << "before delete agent " << endl;
    delete agent;
    // cout << "after  delete agent " << endl;
    agentsChanged_ = true;
}

void PropagatorClass::addAgent(Agent* agent) {
    int id = agent->getAgentId();
    map<int, Agent*>::iterator it = agentMap_.find(id);
    if (it != agentMap_.end()) {
        cerr << "Warning: replacing an existing agent " << id << endl;
    } 
    agentMap_[id] = agent;
    agentsChanged_ = true;
    agent->addCommunicator(communicator_);
    agent->setPropagator(this);
}

Agent* PropagatorClass::getAgent(int agentId) {
    if (agentMap_.find(agentId) == agentMap_.end()) {
        return NULL;
    } else {
        return agentMap_[agentId];
    }
}

void PropagatorClass::updateAgents() {
    if (agentsChanged_) {
        agents_.clear();
        for (map<int, Agent*>::iterator it = agentMap_.begin(); it != agentMap_.end(); ++it) {
            agents_.push_back(it->second);
        }
        composer_->updateOrchestra(agents_);
        agentsChanged_ = false;
    }
}
