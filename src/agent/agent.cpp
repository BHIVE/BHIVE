/**
 * @file agent.cpp
 *
 *	Implementation of the agent class
 *
 * @date Oct 14th, 2009   last edited: Oct 14th, 2009
 *
 * @author Michael Sneddon
 */

#include "agent.hh"


using namespace Hive;

unsigned int Agent::AgentCounter=0;

/// constructor
Agent::Agent() {
	/// unique identifyer of this agent
	agent_id=AgentCounter;
	parent_id=0;

	//children_id = 0;

	/// database of the agent
	db = new Database();

	/// communicator for exchanging messages with the agent's parent agent andchildren
	//c = 0;

	/// message queue; inbox of an agent
	//message_queue=0;

	/// vector of simulators
	//simulators = 0;

	/// actionset. a message tells the agent to execute an action from this set
	//action_set = 0;

	/// message outbox of agent.
	//outbox = 0;

	/// vector of pointers to message_generators
	//message_generators = 0;


	//Increase the global count of agents
	cerr<<"# Agent:"<<agent_id<<" is alive."<<endl;
	AgentCounter++;

	c = NULL;
}

Agent::Agent(int agent_id) {
	cerr<<"# Agent:"<<agent_id<<" is alive."<<endl;
	this->agent_id=agent_id;
	db = new Database();
	AgentCounter++;
	c = NULL;
}

/// destructor
Agent::~Agent() {
	cerr<<"# Agent:"<<this->agent_id<<" is killing itself."<<endl;
	// remove the database
	delete db;
	// remove the action set
	for (unsigned int i=0; i<action_set.size(); i++) {
		delete action_set[i];
		action_set[i] = NULL;
	}
	// remove the simulators
	for (unsigned int i=0; i<simulators.size(); i++) {
		delete simulators[i];
		simulators[i] = 0;
	}
	// remove the message generators
	for (unsigned int i=0; i<message_generators.size(); i++) {
		delete message_generators[i];
		message_generators[i]=NULL;
	}
	// remove message generators for initialisation
	for (unsigned int i=0; i<message_generators_for_initialisation.size(); i++) {
		delete message_generators_for_initialisation[i];
		message_generators_for_initialisation[i] = NULL;
	}
	// remove messages in the outbox
	for (unsigned int i=0; i<outbox.size(); i++) {
		Data *d;
		outbox[i]->getArgument(d,0);
		delete d;
		delete outbox[i];
		outbox[i] = NULL;
	}
	// remove messages in the message queue
	for (unsigned int i=0; i<this->message_queue.size(); i++) {
		Message *m = message_queue.front();
		Data *d;
		m->getArgument(d,0);
		message_queue.pop();
		delete m;
		delete d;
	}
	cerr << "# done killing Agent " << this->agent_id << endl;
}


void Agent::setParent(int parent_id)
{
	this->parent_id=parent_id;
}


void Agent::addChild(int child_id)
{
	this->children_id.push_back(child_id);
}


void Agent::addAction(Action* a)
{
	action_set.push_back(a);
}


void Agent::addCommunicator(Communicator *c)
{
	this->c = c;
}

void Agent::addMessageGenerator(MessageGenerator *msg) {
	message_generators.push_back(msg);
}

void Agent::addMessageGeneratorForInitialisation(MessageGenerator *msg) {
	this->message_generators_for_initialisation.push_back(msg);
}

void Agent::addSimulator(Simulator *s)
{
	simulators.push_back(s);
}

void Agent::addSpecialAgent(int special_id) {
	this->special_agents_id.push_back(special_id);
}

Hive::Database* Agent::getDatabase()
{
	return this->db;
}


int Agent::getParentId()
{
	return this->parent_id;
}

int Agent::getAgentId()
{
	return this->agent_id;
}


int Agent::getNumOfChildren()
{
	return children_id.size();
}

int Agent::getChildAgentId(int childIndex)
{
	return children_id.at(childIndex);
}

bool Agent::isChildAgent(int agentId)
{
	for(unsigned int k=0; k<children_id.size(); k++)
		if(agentId==children_id.at(k)) return true;
	return false;
}

void Agent::init() {
	for (unsigned int i=0; i<this->message_generators_for_initialisation.size(); i++)
		if (this->message_generators_for_initialisation.at(i)->isMessageForParent() &&
			!this->message_generators_for_initialisation.at(i)->isMessageForSpecialAgent())
			this->message_generators_for_initialisation.at(i)->placeMessage(this->parent_id);
	for (int i=outbox.size()-1; i>=0; i--) {
		Message *m = outbox[i];
		outbox.pop_back();
		c->sendMessage(m, m->getDestinationID());
	}

	// send messages to output agent
	for (unsigned int i=0; i<this->message_generators.size(); i++)
		if (message_generators.at(i)->isMessageForSpecialAgent())
			for (unsigned int j=0; j<this->special_agents_id.size(); j++)
				message_generators.at(i)->placeMessage(special_agents_id[j]);

	for (int i=outbox.size()-1; i>=0; i--) {
		Message *m = outbox[i];
		outbox.pop_back();
		c->sendMessage(m,m->getDestinationID());
	}
}

void Agent::outputCurrentState(OutputWriter *ow)
{
	cout<<"Agent:"<<this->agent_id<<" is outputting current state"<<endl;
}


void Agent::propagate(double time)
{
//	cerr << "# agent " << this->agent_id << " is evaluating message queue" << endl;
	this->evaluateMessageQueue();

//	cerr << "# agent " << this->agent_id << " is sending messages to children" << endl;
	// generate messages to children
	for(unsigned int i=0; i<this->message_generators.size(); i++)
		if (!message_generators.at(i)->isMessageForParent() && !message_generators.at(i)->isMessageForSpecialAgent())
			for (unsigned int j=0; j<this->children_id.size(); j++)
				message_generators.at(i)->placeMessage(children_id[j]);

	// send messages
	for (int i=outbox.size()-1; i>=0; i--) {
		Message *m = outbox[i];
		outbox.pop_back();
		//		cerr << "# in agent send message" << m->getActionId() << endl;
		c->sendMessage(m, m->getDestinationID());
	}


//	cerr << "# agent " << this->agent_id << " is telling children to prop." << endl;
	// propagate children
	for (unsigned int i=0; i<this->children_id.size(); i++ ) {
		//		cerr << children_id.at(i) << endl;
		c->propogateAgent(children_id.at(i),time);
	}

	// evaluate messages from children
//	cerr << "# agent " << this->agent_id << " is evaluating its messsage queue" << endl;
	this->evaluateMessageQueue();

//	cerr << "# agent " << this->agent_id << " is propagating its simulators" << endl;
	for (unsigned int i=0; i<simulators.size(); i++) {
		simulators.at(i)->synchroniseWithDatabase();
		simulators[i]->step(time);
	}

//	cerr << "# agent " << this->agent_id << " is sending messages to parent" << endl;
	// generate messages for parent
	for(unsigned int i=0; i<this->message_generators.size(); i++)
		if (message_generators.at(i)->isMessageForParent() && !message_generators.at(i)->isMessageForSpecialAgent())
			message_generators.at(i)->placeMessage(this->parent_id);
	// send messages to parent
	for (int i=outbox.size()-1; i>=0; i--) {
		Message *m = outbox[i];
		//cerr << "# in agent prop send message " << m->getActionId() << endl;
		outbox.pop_back();
		c->sendMessage(m, m->getDestinationID());
	}

//	cerr << "# agent " << this->agent_id << " is sending messages to special_agent" << endl;
	// at the agent has done everything needed for the simulation it
	// sends messages to the special_agents. for the beginning this will
	// only be the outputagent
	// send messages to output agent
	for (unsigned int i=0; i<this->message_generators.size(); i++)
		if (message_generators.at(i)->isMessageForSpecialAgent())
			for (unsigned int j=0; j<this->special_agents_id.size(); j++)
				message_generators.at(i)->placeMessage(special_agents_id[j]);

	for (int i=outbox.size()-1; i>=0; i--) {
		Message *m = outbox[i];
		outbox.pop_back();
		c->sendMessage(m,m->getDestinationID());
	}
}


void Agent::setupDatabase()
{
	cout<<"#Agent:"<<this->agent_id<<" is setting up its database"<<endl;
}


void Agent::evaluateMessageQueue()
{
	int message_queue_size = message_queue.size();
//	cout<<"Agent:"<<this->agent_id<<" is evaluating its message queue"<<endl;
//	cerr << "# there are " << message_queue.size() << " messages in " << this->agent_id << " message queue" << endl;
	for (unsigned int i=0; i<message_queue_size; i++) {
		Data *d;
		Message *m = message_queue.front();
		m->getArgument(d,0);
//		cerr << "#in agent " << this->getAgentId() << " evaluate messagequeue" << d->getType() << endl;
		//cerr << "# in Agent::evaluateMessageQueue " <<  m->getActionId() << endl;
		action_set[m->getActionId()]->fire(d);
		message_queue.pop();
		// delete message and data
		delete m;
		delete d;
	}
}

