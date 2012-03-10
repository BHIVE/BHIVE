#include "outputagentfactory.hh"

using namespace Hive;
using namespace Heatbug;

OutputAgentFactory::OutputAgentFactory(){
	this->number_bugs     = 0;
	this->output_interval = 0;
	this->binaryOutput    = 0;
	this->outputSuffix    = "";
}

OutputAgentFactory::~OutputAgentFactory() { }

Agent* OutputAgentFactory::createAgent() {
	// create new output agent
	Agent *shakespeare  = new Agent();
	Database *db        = shakespeare->getDatabase();

	// add number of bugs and length of output interval to shakespeare's database
	IntegerData *nb = new IntegerData("number_bugs",number_bugs);
	db->addData(nb);

	DoubleData *oi = new DoubleData("output_interval", output_interval);
	db->addData(oi);

	// we just need to add the actions to the agents.
	// the output-agent does not possess any message generators or simulators.
	this->addActionsToAgentsActionSet(shakespeare);

	return shakespeare;
}

Agent* OutputAgentFactory::createAgent(Action **as) {
	throw HiveException("method not implemented", "Agent* OutputAgentFactory::createAgent(Action **as)");
}

Agent* OutputAgentFactory::duplicateAgent(Agent *ag) {
	throw HiveException("method not implemented", "Agent* OutputAgentFactory::duplicateAgent(Agent *ag)");
}

// does not do anything
void OutputAgentFactory::finalise() { }

void OutputAgentFactory::addActionsToAgentsActionSet(Agent *ag) {
	SpecialAgentReceiveBugPositionsAction *sarpa = new SpecialAgentReceiveBugPositionsAction();
	sarpa->setAgent(ag);
	sarpa->prepareOutputfile("bug_grid_data"+this->outputSuffix+".dat", this->binaryOutput);
	ag->addAction(sarpa);

	SpecialAgentReceiveTemperatureFieldAction *sartfa = new SpecialAgentReceiveTemperatureFieldAction();
	sartfa->setAgent(ag);
	sartfa->prepareOutputfile("temperature_grid_data"+this->outputSuffix+".dat", this->binaryOutput);
	ag->addAction(sartfa);

	SpecialAgentUpdateBugHappinessAction *saubha = new SpecialAgentUpdateBugHappinessAction();
	saubha->setAgent(ag);
	saubha->prepareOutputfile("bug_happiness_data"+this->outputSuffix+".dat", this->binaryOutput);
	ag->addAction(saubha);
}

void OutputAgentFactory::addSimulatorToAgent(Agent *ag) { }

void OutputAgentFactory::addMessageGeneratorsToAgent(Agent *ag) { }
