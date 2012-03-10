#include "outputagentfactory.hh"

using namespace Hive;
using namespace ChemoPop;

OutputAgentFactory::OutputAgentFactory() {
		this->numbercells = 0;
		this->output_interval = 0;
		this->binaryOutput=false;
		this->outputSuffix="";
}

OutputAgentFactory::OutputAgentFactory(InputSystemReader* isr) { }

OutputAgentFactory::~OutputAgentFactory() { }

Agent* OutputAgentFactory::createAgent() {
	SpecialAgent *ibn = new SpecialAgent();
	Database *db = ibn->getDatabase();

	IntegerData *numberofcells = new IntegerData("numberofcells",this->numbercells);
	db->addData(numberofcells->getName(), numberofcells);

	TVectorData<TVectorData<double>* > *cell_information = new TVectorData<TVectorData<double>* > ("celldatamatrix",
																								   "tvector_doublematrix" );
	cell_information->reserveSize(this->numbercells);
	for (int i=0; i<this->numbercells; i++) {
		cell_information->at(i) = new TVectorData<double> ("infoline", "tvector_double");
		/// at this point i use the information that there is only one ligand and one accumulated ligand !
		cell_information->at(i)->reserveSize(5);
	}
	db->addData(cell_information->getName(), cell_information);

	IntegerData *numberofblindagents = new IntegerData("numberofblindagents", this->numberblindagents);
	db->addData(numberofblindagents->getName(), numberofblindagents);


	IntegerData *changeInBlindAgentNumberSinceLastOutput = new IntegerData("change_in_blind_agent_number_since_last_output", 0);
	db->addData(changeInBlindAgentNumberSinceLastOutput);


	// add field to accumulate blind agent's data
	TVectorData<TVectorData<double>* > *blind_agent_information = new TVectorData<TVectorData<double>* > ("blindagentdatamatrix",
																										  "tvector_doublematrix");
	blind_agent_information->reserveSize(this->numberblindagents);
	for (int i=0; i<this->numberblindagents; i++) {
		blind_agent_information->at(i) = new TVectorData<double> ("infoline", "tvector_double");
		blind_agent_information->at(i)->reserveSize(4);
	}
	db->addData(blind_agent_information->getName(), blind_agent_information);

	this->addActionsToAgentsActionSet(ibn);

	return ibn;
}

Agent* OutputAgentFactory::createAgent(Action **as) {
	Agent *ibn = new Agent();
	return ibn;
}

Agent* OutputAgentFactory::duplicateAgent(Agent *ag) {
	Agent *ibn = new Agent();
	return ibn;
}

void OutputAgentFactory::finalise() {
	// i don't think there is anything to do here ...
}

void OutputAgentFactory::setNumberBlindAgents(int nba) {
	this->numberblindagents = nba;
}

void OutputAgentFactory::setNumberCells(int nc) {
	this->numbercells = nc;
}

void OutputAgentFactory::addActionsToAgentsActionSet(Agent *ag) {
	SpecialAgentReceiveCellPositionsAction *sarcpa = new SpecialAgentReceiveCellPositionsAction();
	ag->addAction(sarcpa);
	sarcpa->setAgent(ag);
	/// binary = true / false
	sarcpa->prepareOutputfile("cell_positions"+this->outputSuffix+".dat", this->binaryOutput);

	SpecialAgentAccumulateCellDataAction *saacda = new SpecialAgentAccumulateCellDataAction();
	ag->addAction(saacda);
	saacda->setAgent(ag);
	saacda->prepareOutputfile("cell_data"+this->outputSuffix+".dat", this->binaryOutput);

	SpecialAgentAccumulateBlindAgentDataAction *saabada = new SpecialAgentAccumulateBlindAgentDataAction();
	ag->addAction(saabada);
	saabada->setAgent(ag);
	saabada->prepareOutputfile("blind_agent_data"+this->outputSuffix+".dat", this->binaryOutput);

	SpecialAgentUpdateBlindAgentCountAction * saubaca = new SpecialAgentUpdateBlindAgentCountAction();
	ag->addAction(saubaca);
	saubaca->setAgent(ag);

	SpecialAgentOutputGridAction *saoga = new SpecialAgentOutputGridAction();
	ag->addAction(saoga);
	saoga->setAgent(ag);
	saoga->prepareOutputfile("world_state"+this->outputSuffix+".dat",this->binaryOutput);

}


// it should be forbidden to add simulators and messagegenerators to an output-agent
void OutputAgentFactory::addSimulatorToAgent(Agent *ag) { }

void OutputAgentFactory::addMessageGeneratorsToAgent(Agent *ag) { }
