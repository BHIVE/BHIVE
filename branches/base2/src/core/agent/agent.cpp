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


/// constructor
Agent::Agent() {
	/// unique identifyer of this agent
	agent_id=Registrar::getSystemRegistrar()->reserveNewAgentID();
	parent_id=0;
	is_special_agent=false;

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



	IntegerData *agentId = new IntegerData("agent_id", this->agent_id);
	db->addData(agentId->getName(), agentId);

	c = NULL;


	//Registrar::getSystemRegistrar()->registerNewAgent(this);
	//cerr<<"# Agent:"<<agent_id<<" is alive."<<endl;
}

//Agent::Agent(int agent_id) {
//	//cerr<<"# Agent:"<<agent_id<<" is alive."<<endl;
//	this->agent_id=agent_id;
//	db = new Database();
//	AgentCounter++;
//	c = NULL;
//}

/// destructor
Agent::~Agent() {
//	cerr<<"# Agent:"<<this->agent_id<<" is killing itself."<<endl;

//	// before we die, send off our final words and prayers
//	for (int i=outbox.size()-1; i>=0; i--) {
//		Message *m = outbox[i];
//		outbox.pop_back();
//		//		cerr << "# in agent send message" << m->getActionId() << endl;
//		c->sendMessage(m, m->getDestinationID());
//	}

	// delete my database
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
	//cerr << "# done killing Agent " << this->agent_id << endl;
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
	action_lookup_table[a->getIdentifyer()] = action_set.size()-1;
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

unsigned int Agent::getNumOfSimulators() const { return this->simulators.size(); }

Simulator * Agent::getSimulator(int s) { return simulators.at(s); }

void Agent::replaceSimulator(int s, Simulator *newSim)
{
	Simulator *oldSim = simulators.at(s);
	simulators.at(s) = newSim;
	newSim->setAgent(this);  // setting the agent will initialize this simulator
	delete oldSim;
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

bool Agent::removeChildAgent(int agentId)
{
	for(unsigned int k=0; k<children_id.size(); k++)
		if(agentId==children_id.at(k)) {
			if(k==children_id.size()-1) children_id.pop_back();
			else {
				children_id.at(k) = children_id.at(children_id.size()-1);
				children_id.pop_back();
			}
			return true;
		}
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
}

void Agent::outputCurrentState(OutputWriter *ow)
{
	cout<<"Agent:"<<this->agent_id<<" is outputting current state"<<endl;
}


void Agent::propagate(double time)
{
	//cerr << "#Agent::propagate() agent " << this->agent_id << " is evaluating message queue" << endl;
	this->evaluateMessageQueue();

	//	cerr << "# agent " << this->agent_id << " is sending messages to children" << endl;
	// generate messages to children
	for(unsigned int i=0; i<this->message_generators.size(); i++)
		if (!message_generators.at(i)->isMessageForParent() && !message_generators.at(i)->isMessageForSpecialAgent()) {
			int number_children = this->children_id.size();
			for (unsigned int j=0; j<number_children; j++)
				message_generators.at(i)->placeMessage(children_id[j]);
		}

	// send messages
	for (int i=outbox.size()-1; i>=0; i--) {
		Message *m = outbox[i];
		outbox.pop_back();
		//		cerr << "# in agent send message" << m->getActionId() << endl;
		c->sendMessage(m, m->getDestinationID());
	}


	//cerr << "# Agent::propagate() agent " << this->agent_id << " is telling children to prop." << endl;
	// propagate children
	unsigned int number_children = this->children_id.size();
	for (unsigned int i=0; i<number_children; i++ ) {
		//		cerr << children_id.at(i) << endl;
		c->propogateAgent(children_id.at(i),time);
	}

	// evaluate messages from children
	//cerr << "#  Agent::propagate() agent " << this->agent_id << " is evaluating its messsage queue" << endl;
	this->evaluateMessageQueue();

	//	cerr << "# agent " << this->agent_id << " is propagating its simulators" << endl;
	for (unsigned int i=0; i<simulators.size(); i++) {
//		cerr << i << endl;
		simulators.at(i)->synchroniseWithDatabase();
		simulators[i]->step(time);
	}

	//cerr << "#  Agent::propagate() agent " << this->agent_id << " is sending messages to parent" << endl;
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

	//cerr << "#  Agent::propagate()  agent " << this->agent_id << " is sending messages to special_agent" << endl;
	// if the agent has done everything needed for the simulation it
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


void Agent::generateMessagesToParent()
{
	for(unsigned int i=0; i<this->message_generators.size(); i++)
		if (message_generators.at(i)->isMessageForParent() && !message_generators.at(i)->isMessageForSpecialAgent())
			message_generators.at(i)->placeMessage(this->parent_id);
}


void Agent::setupDatabase()
{
	//cout<<"#Agent:"<<this->agent_id<<" is setting up its database"<<endl;
}


void Agent::evaluateMessageQueue()
{
	int actionId;
	int actionPos;
	int message_queue_size = message_queue.size();
//	cout<<"Agent:"<<this->agent_id<<" is evaluating its message queue"<<endl;
//	cerr << "# there are " << message_queue.size() << " messages in " << this->agent_id << " message queue" << endl;
	for (unsigned int i=0; i<message_queue_size; i++) {
		Data *d;
		Message *m = message_queue.front();
		m->getArgument(d,0);
//		cerr << "#in agent " << this->getAgentId() << " evaluate message queue " << d->getType() << endl;
//		cerr << "# in Agent::evaluateMessageQueue " <<  m->getActionId() << endl;
		// get the action Id from the message
		actionId = m->getActionId();
		// lookup position at which action is stored in agent
		actionPos = this->action_lookup_table[actionId];
		// execute action
		action_set[actionPos]->fire(d);
		message_queue.pop();
		// delete message and data
		delete m;
		delete d;
	}
}




void Agent::copyDatabaseInformationFromExistingAgent(Agent *aFrom)
{
	Database * originDB = aFrom->db;
	for(unsigned int k=0; k<originDB->size(); k++) {
		if(!db->existsDataItem(originDB->at(k)->getName())) {
			throw(HiveException("Cannot copy data item from agent, data: "+originDB->at(k)->getName()+" does not exist in destination db.",
					"Agent::copyDatabaseInformationFromExistingAgent(Agent *aFrom)"));
		}
		originDB->at(k)->copyContentsInto(db->getDataItem(originDB->at(k)->getName()));
	}
}
