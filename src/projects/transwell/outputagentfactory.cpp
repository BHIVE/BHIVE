#include "outputagentfactory.hh"

using namespace Transwell;


OutputAgentFactory::OutputAgentFactory():AgentFactory() {
	cerr << "# hello, i am the transwell's OutputAgentFactory" << endl;
	this->numbercells = 0;
}

OutputAgentFactory::OutputAgentFactory(InputSystemReader *isr):AgentFactory(isr) {
	cerr << "#hello, i am the transwell's OutputAgentFactory" << endl;
	this->numbercells = 0;
}

OutputAgentFactory::~OutputAgentFactory() {}

Agent* OutputAgentFactory::createAgent() {
	SpecialAgent *fox_mulder = new SpecialAgent();

	// meta database. i will store the meta information that might
	// be needed in a meta database of the special_Agent.
	Hive::Database *meta = fox_mulder->getMetaDatabase();
	IntegerData *total_number_of_cells  = new IntegerData("total_number_of_cells", this->numbercells);
	meta->addData("total_number_of_cells", total_number_of_cells);
	IntegerData *number_of_adds = new IntegerData("number_of_adds",0);
	meta->addData("number_of_adds", number_of_adds);
	DoubleData *global_time = new DoubleData("global_time", 0.0);
	meta->addData("global_time", global_time);
	DoubleData *time_step = new DoubleData("time_step", 5.0);
	meta->addData("time_step", time_step);
	IntegerData *number_between_outputs = new IntegerData("number_between_outputs",10);
	meta->addData("number_between_outputs", number_between_outputs);
	IntegerData *counter = new IntegerData("counter", 9);
	meta->addData("counter", counter);

	Hive::Database *db = fox_mulder->getDatabase();
	TVectorData<double> *total_conc = new TVectorData<double> ("total_conc", "tvectordata_double");
	total_conc->reserveSize(15);
	db->addData("total_conc", total_conc);

	this->addActionsToAgentsActionSet(fox_mulder);

	return fox_mulder;
}

Agent* OutputAgentFactory::createAgent(Action **as) {
	SpecialAgent *fox_mulder = new SpecialAgent();
	cerr << "#OutputAgentFactory::createAgent(Action **as): only creating dummy" << endl;
	return fox_mulder;
}

Agent* OutputAgentFactory::duplicateAgent(Agent *ag) {
	SpecialAgent *fox_mulder = new SpecialAgent();
	cerr << "#OutputAgentFactory::duplicateAgent(Agent *ag) not yet implemented" << endl;
	return fox_mulder;
}

void OutputAgentFactory::finalise() { }

void OutputAgentFactory::setNumberCells(int i) {
	this->numbercells = i;
}

void OutputAgentFactory::addActionsToAgentsActionSet(Agent *ag) {
	Action *a1 = new SpecialAgentAccumulateDataAction();
	((SpecialAgentAccumulateDataAction* ) a1)->prepareOutputfile("tcell_concentrations.txt");
	a1->setAgent(ag);
	ag->addAction(a1);
}

void OutputAgentFactory::addSimulatorToAgent(Agent *ag) { }

void OutputAgentFactory::addMessageGeneratorsToAgent(Agent *ag) { }

