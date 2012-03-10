/**
 * registrar.cpp
 *
 *  Created on: Jun 18, 2010
 *      Author: msneddon
 */



#include "registrar.hh"


#include <sstream>
//#include "../agent/special_agent.hh"


using namespace std;
using namespace Hive;



/////////////////////////////////////////////////////////////
/// First implement the static functionality

Registrar * Registrar::SystemRegistrar = NULL;
unsigned int Registrar::AgentIDCounter = 0;
queue <unsigned int> Registrar::AvailableAgentIDs;

bool Registrar::createSystemRegistrar()
{
	if(Registrar::SystemRegistrar==NULL) {
		Registrar::SystemRegistrar = new Registrar();
	}
	return false;
}



Registrar * Registrar::getSystemRegistrar()
{
	return Registrar::SystemRegistrar;
}


bool Registrar::deleteSystemRegistrar()
{
	if(Registrar::SystemRegistrar!=NULL) {

		map<int,Agent*>::iterator iter = Registrar::SystemRegistrar->agentMap.begin();
		while (iter != Registrar::SystemRegistrar->agentMap.end()) {
			delete iter->second;
			iter++;
		}
		delete Registrar::SystemRegistrar;
		Registrar::SystemRegistrar = NULL;
		return true;
	}

	//return false if the registrar was already null, which means
	//we couldn't actually do this.
	return false;
}



Registrar::Registrar()
{

}

Registrar::~Registrar()
{

}

void Registrar::registerAgentFactory(AgentFactory *agentFactory) {
	this->agentFactories.push_back(agentFactory);
}
int Registrar::getNumberOfAgentFactories() const {
	return this->agentFactories.size();
}
AgentFactory * Registrar::getAgentFactory(int i) {
	return this->agentFactories.at(i);
}


unsigned int Registrar::getNumberOfAgents() const {
	return this->agentMap.size();
}

Agent *Registrar::getAgent(unsigned int agentID)
{
	agentMapIter = this->agentMap.find(agentID);
	if(agentMapIter==agentMap.end()) {
		stringstream id; id << agentID;
		throw(HiveException("No agent found with ID: "+id.str(),"Registrar::getAgent(agentID)"));
	}

	return (*agentMapIter).second;
}



void Registrar::registerNewAgentAndSendMessageToSpecialAgent(Agent *newAgent, Message *m)
{
	registerNewAgent(newAgent);

	//add new agent as child to its parent
	agentMap.find(newAgent->getParentId())->second->addChild(newAgent->getAgentId());

	//for now, only send the message to the only special agent that exists
	//note that we will have the problem of matching messages to special agents
	//when we have more than one special messages.  If there are no special agents,
	//just throw the message in the trash.
	if (specialAgentList.size()>0)
		((Agent *)specialAgentList.at(0))->placeMessageOnMessageQueue(m);
	else
		delete m;
}


void Registrar::registerNewAgent(Agent *newAgent)
{
//	cerr<<" # registering new agent with ID: "<< newAgent->getAgentId() <<endl;
	this->agentMap.insert(pair<int,Agent*>(newAgent->getAgentId(),newAgent));
}

void Registrar::registerNewSpecialAgent(SpecialAgent *newSpecialAgent)
{
	if (specialAgentList.size()!=0) {
		throw(HiveException("Registrar can only store one SpecialAgent for now!!","Registrar::registerNewSpecialAgent(newSpecialAgent)"));
	}
	this->specialAgentList.push_back(newSpecialAgent);
}




unsigned int Registrar::reserveNewAgentID()
{

	// Should create a queue here that allows us to get a free
	// AgentID from a dead agent

	if(this->AvailableAgentIDs.size()==0) {

		if(this->agentMap.size()>50000) {
			throw(HiveException("Fibonacci rabbits!!","Registrar::reserveNewAgentID()"));
		}

		return Registrar::AgentIDCounter++;
	}
	else {
		unsigned int recycledAgentId = AvailableAgentIDs.front();
		AvailableAgentIDs.pop();
		return recycledAgentId;
	}
}





void Registrar::killAgentAndSendMessageToSpecialAgent(unsigned int agentID, Message *m)
{
	//cerr<<"# killing agent with ID: "<<agentID<<endl;
	killAgent(agentID);

	//cerr<<"done killing"<<endl;
	//for now, only send the message to the only special agent that exists
	//note that we will have the problem of matching messages to special agents
	//when we have more than one special messages.  If there are no special agents,
	//just throw the message in the trash.
	if (specialAgentList.size()>0)
		((Agent *)specialAgentList.at(0))->placeMessageOnMessageQueue(m);
	else
		delete m;

	//cerr<<"done sending message"<<endl;
}



void Registrar::killAgent(unsigned int agentID)
{
	agentMapIter = this->agentMap.find(agentID);
	if(agentMapIter==agentMap.end()) {
		stringstream id; id << agentID;
		throw(HiveException("No agent found with ID: "+id.str(),"Registrar::killAgent(agentID)"));
	}

	//First, grab the parent agent ID
	unsigned int pID = (*agentMapIter).second->getParentId();


	//Second, kill the agent's family (i.e. children of this agent)
	for(int k=0; k<(*agentMapIter).second->getNumOfChildren(); k++) {
		unsigned int childID = (*agentMapIter).second->getChildAgentId(k);
		killAgent(childID);
	}

	//Be nice and let the agent generate any final requests
	(*agentMapIter).second->generateMessagesToParent();

	//Next, kill the agent himself
	delete agentMapIter->second;
	agentMap.erase(agentMapIter);


	//Now grab the parent so we can update its child list
	agentMapIter = this->agentMap.find(pID);
	if(agentMapIter==agentMap.end()) {
		stringstream id; id << agentID;
		throw(HiveException("Agent with ID: "+id.str()+" does not have a valid parent","Registrar::killAgent(agentID)"));

	}
	(*agentMapIter).second->removeChildAgent(agentID);


	/// @todo Recycle agent IDs?  if so, uncomment
	// recycle the agent ID so that we don't run out of AgentIDs
	//this->AvailableAgentIDs.push(agentID);

	//And we're done.
}




