/**
 * @file dummyAgent.cpp
 *
 *implementation of dummyagent.hh
 *
 *@date Oct 14, 2009 last edited 10-15-2009 garrit
 *
 *@author garrit jentsch
 */

#include "dummycomposer.hh"
#include "../../exception/exception.hh"
#include "../../util/util.hh"

using namespace Hive;
using namespace Dummy;


DummyComposer::DummyComposer() { cout << "# hello i am the dummycomposer" << endl;}

DummyComposer::~DummyComposer() {}

void DummyComposer::setupSimulation() {
	try {
		// start by creating the factories
		this->initializeAgentFactories();
		// creat Agents
		maestro = this->createAgent(*factories[0]);
		orchestra.push_back(maestro);
		Agent *bass_player = this->createAgent(*factories[0]);
		orchestra.push_back(bass_player);
		// setup hierarchy among agent
		this->setupAgentHierarchy();
	} catch (HiveException& e) {
		e.addTraceException("DummyComposer::setupSimulation()");
		throw e;
	}
}

void DummyComposer::initializeAgentFactories() {
	DummyAgentFactory *dummyfactory = new DummyAgentFactory();
	factories.push_back(dummyfactory);
	Util::convertToDouble("adf");
	throw HiveException("Random Error","DummyComposer::initializeAgentFactories()");

}

void DummyComposer::setupAgentHierarchy() {
	//
	orchestra[0]->addChild(orchestra[1]->getAgentId());
	orchestra[1]->setParent(orchestra[0]->getAgentId());

	SerialCommunicator *c = new SerialCommunicator();
	// add communicators to the orchestra members
	for (unsigned int i=0; i<orchestra.size(); i++) {
		c->addAgent(orchestra[i]);
		orchestra[i]->addCommunicator(c);
	}

}

Agent* DummyComposer::createAgent(AgentFactory& af) {
	// create an agent
	Agent *ag = af.createAgent();


	// the following code is for testing the action
	// first we set up two data items

//	Data *dat2 = new DoubleData("hans",0.232);
	// fire the action twice with the argument dat
//	act->fire(dat);
//	act->fire(dat);
	// check whether the firing was successful
//	ag->getDatabase()->getDataItem("waveNumber")->printContent(cerr);
	// fire with the wrong type of argument
//	act->fire(dat2);

	return ag;
}

Agent* DummyComposer::getTopLevelAgent() {
	return maestro;
}

