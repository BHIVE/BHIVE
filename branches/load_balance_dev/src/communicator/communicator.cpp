
#include <set>

#include "communicator.hh"
#include "../parallel/parallel.hh"

using namespace std;
using namespace Hive;

Communicator::Communicator() {}

Communicator::~Communicator() {}

void Communicator::sendMessage(Message* message) {
    if (message->getDestinationNode() == Hive::mpirank()) {
        sendLocalMessage(message);
    } else {
        sendRemoteMessage(message);
    }
}

void Communicator::addAgent(Agent *agent) {
    cerr << "Error: addAgent() not supported; use PropogatorClass instead" << endl;
	// agentMap->[agent->getAgentId()] = agent;
}

void Communicator::setupAgentMap(map<int, Agent*>* agentMap) {
    agentMap_ = agentMap;
}

void Communicator::setupNeighbors(const vector<int>& neighbors) {
    neighbors_.clear();
    set<int> seen;
    seen.insert(mpirank());
    if (!neighbors.empty()) {
        for (unsigned int i = 1; i < neighbors.size(); ++i) {
            if (seen.find(neighbors[i]) == seen.end()) {
                neighbors_.push_back(neighbors[i]);
                seen.insert(neighbors[i]);
            }
        }
    }
}

void Communicator::parallelCommunicate() {
    if (mpisize() == 1) return;
    // printf("[%d] outbox = %d \n", mpirank(), outbox_.size());
    parallel_send_recv(outbox_, inbox_, neighbors_);
    // printf("[%d] inbox  = %d \n", mpirank(), inbox_.size());
    for (unsigned int i = 0; i < inbox_.size(); ++i) {
        Message* message = inbox_[i];
        Data* data;
        message->getArgument(data, 0);
        (*agentMap_)[message->getDestinationID()]->placeMessageOnMessageQueue(message);
    }
}

void Communicator::clearQueues() {
    if (mpisize() > 1) {
        inbox_.clear();
        outbox_.clear();
    }
}

void Communicator::sendLocalMessage(Message* message) {
    // printf("[%d] sendLocal  destId = %d\n", mpirank(), message->getDestinationID());
    (*agentMap_)[message->getDestinationID()]->placeMessageOnMessageQueue(message);
}

void Communicator::sendRemoteMessage(Message* message) {
    // printf("[%d] sendRemote destId = %d destNode = %d\n", mpirank(), message->getDestinationID(), message->getDestinationNode());
    // cout << "message->getSourceID() = " << message->getSourceID() << endl;
    outbox_.push_back(message);
}
