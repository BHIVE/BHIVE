#include "special_agent.hh"

using namespace Hive;

SpecialAgent::SpecialAgent():Agent()
{
	is_special_agent = true;
	this->meta = new Database();
	this->internalclock = 0;

	//Registration is handled in the composer
	//Registrar::getSystemRegistrar()->registerNewSpecialAgent(this);
}

//SpecialAgent::SpecialAgent(int id) : Agent(id){ is_special_agent = true; this->meta = new Database(); }

SpecialAgent::~SpecialAgent() {
	// i only need to kill the meta data base the rest is done by the destructor of the agent?!
	cerr << "# Special_agent " << this->agent_id << " is killing itself" << endl;
	delete meta;
	cerr << "# done with the special agent" << endl;
}


void SpecialAgent::evaluateMessageQueue() {
	if (this->message_queue.size() > 0)
		this->updateInternalClock();
	Agent::evaluateMessageQueue();
}


void SpecialAgent::setParent(int parent_id) {
	throw HiveException("can not assign a parent to a special agent", "SpecialAgent::setParent(int parent_id)");
}

void SpecialAgent::addChild(int child_id) {
	throw HiveException("can not add child to a special agent", "SpecialAgent::addChild(int child_id)");
}

void SpecialAgent::addSimulator(Simulator *s) {
	throw HiveException("a special agent can not have any simulators", "SpecialAgent::addSimulator(Simulator *s)");
}

int SpecialAgent::getParentId() {
	throw HiveException("a special agent does not have a parent", "SpecialAgent::getParentId()");
}

Database* SpecialAgent::getMetaDatabase() {
	return this->meta;
}

int SpecialAgent::getNumOfChildren() {
	throw HiveException("a special agent does not have any children", "SpecialAgent::getNumOfChildren()");
}

int SpecialAgent::getChildAgentId(int childIndex) {
	throw HiveException("a special agent does not have any children", "SpecialAgent::getChildAgentId(int childIndex)");
}

bool SpecialAgent::isChildAgent(int agentID) {
	throw HiveException("a special agent does not have any children", "SpecialAgent::isChildAgent(int agentID)");
}


void SpecialAgent::propagate(double time) {
	cerr << "# SpecialAgent::propagate(double time) not implemented as of yet" << endl;
}

