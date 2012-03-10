#include "parallel_propagator.hh"

using namespace Hive;

ParallelPropagator::ParallelPropagator(Composer *composer) :
	PropagatorClass(composer)
{
	//cerr << "# hello from the ParallelPropagator constructor" << endl;
}

ParallelPropagator::~ParallelPropagator() {
//	cerr << "# within the ParallelPropagator's destructor" << endl;
}

void ParallelPropagator::go(double dt, double endTime)
{
	// Here is the main simulation loop
	for ( ; currentTime<endTime; currentTime+=dt)
	{

//		cerr<<"hello, I am going."<<endl;
		// STEPS 1-3: PRE MESSAGE SENDING, RECIEVING, AND EVALUATION
		// STEP  4:   RUN SIMULATORS
		// STEPS 6-8: POST MESSAGE SENDING, RECIEVING, AND EVALUATION


		// STEP 1:  tell each agent to place its messages in its outbox,
		// then tell each agent to purge its outbox by sending the messages
		// the communicator will immediately send all messages that are to
		// be delivered within the same node, and collect all messages to
		// be delivered to other nodes into a nice little packag
	//	cerr<<"size:"<<this->agents.size()<<endl;
		for(unsigned int a=0; a<this->agents.size(); a++) {
			agents.at(a)->placePreMessagesInOutbox();
			agents.at(a)->sendMessages();
		}


		// STEP 2:  the communicator will now send messages
		this->communicator->parallelCommunicate();

//
//
		// STEP 3:  agents handle any messages they received
//		cerr << this->agents.size() << endl;
		for(unsigned int a=0; a<this->agents.size(); a++) {
			agents.at(a)->evaluateMessageQueue();
		}
		// remove old messages from communicators message queue ...
		this->communicator->clearQueues();
//
//
//		///////////////////////////////////////////////////////////////////////
//		///////////////////////////////////////////////////////////////////////
//		// STEP 4:  agents are allowed to start their simulators
//		// note: you could do this in multiple threads!
		for(unsigned int a=0; a<this->agents.size(); a++) {
			agents.at(a)->advanceSimulators(dt);
		}
//		///////////////////////////////////////////////////////////////////////
//		//////////////////////////////////////////////////////////////////////
//
//
//
//		// STEP 5:  tell each agent to place its messages in its outbox,
//		// then tell each agent to purge its outbox by sending the messages
//		// the communicator will immediately send all messages that are to
//		// be delivered within the same node, and collect all messages to
//		// be delivered to other nodes into a nice little packag
		for(unsigned int a=0; a<this->agents.size(); a++) {
			agents.at(a)->placePostMessagesInOutbox();
			agents.at(a)->sendMessages();
		}
//
//
//		// STEP 6:  the communicator will now send messages
		this->communicator->parallelCommunicate();
//
//
//
//		// STEP 7:  agents handle any messages they received
		for(unsigned int a=0; a<this->agents.size(); a++) {
			agents.at(a)->evaluateMessageQueue();
		}
		this->communicator->clearQueues();
	}

}

void ParallelPropagator::equilibrate(double time) {

}

void ParallelPropagator::setup() {
	// get the number of agents that the composer has built
	int number_agents = this->comp->getNumberAgents();

	// get the agents that live here
	for (int i=0; i<number_agents; i++)
		this->agents.push_back(comp->getAgent(i));
}



