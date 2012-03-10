/*
 * dummyagentfactory.cpp
 *
 * minimal implementation
 *
 * @date: Oct 14, 2009 last edited 10-15-2009
 * @author: garrit jentsch
 */

#include "dummyagentfactory.hh"
#include "../../agent/data/primitive/primitiveData.hh"
#include "dummywaveaction.hh"
#include "../../communicator/serial/serialcommunicator.hh"
#include "../../simulators/dummy/dummySimulator.hh"
#include "../../simulators/simulator.hh"
#include "dummymessagegenerator.hh"

#include <string>

using namespace Hive;
using namespace Dummy;

DummyAgentFactory::DummyAgentFactory() { cout << "# hello i am the dummyagentfactory" << endl; }

DummyAgentFactory::DummyAgentFactory(InputSystemReader *isr):AgentFactory(isr) {}

Agent* DummyAgentFactory::createAgent() {
	Agent *dummy = new Agent();

	Hive::Database *db = dummy->getDatabase();
	IntegerData *waveNumberData = new IntegerData("waveNumber",5);
	db->addData(waveNumberData->getName(),waveNumberData);

	// create an action
	DummyWaveAction *act = new DummyWaveAction(0);
	// add it to the agent
	dummy->addAction(act);
	// tell action to which agent it belongs ...
	act->setAgent(dummy);

	Simulator *s = new DummySimulator(dummy);
	dummy->addSimulator(s);
	s->setAgent(dummy);

	DummyMessageGenerator *dmg = new DummyMessageGenerator(dummy,false);
	dummy->addMessageGenerator(dmg);

	return dummy;
}

Agent* DummyAgentFactory::createAgent(Action** ac) {
	return NULL;
}

Agent* DummyAgentFactory::duplicateAgent(Agent *ag) {
	return NULL;	
}

void DummyAgentFactory::addActionsToAgentsActionSet(Agent* a) {}

void DummyAgentFactory::addSimulatorToAgent(Agent *ag) {}

void DummyAgentFactory::finalise() {}
