#include "CellAgentFactory.hh"

using namespace std;
using namespace Hive;
using namespace BatchReactor;

CellAgentFactory::CellAgentFactory()  {}
CellAgentFactory::~CellAgentFactory() {}

//WE THINK IT SHOULD WORK LIKE THIS
//Agent* createAgent(vector<string> varTypes, vector<string> varNames, vector<vector<string>> varArgs, vector<MessageGenerators*>,vector<Simulators*>);

Agent* CellAgentFactory::createAgent() {
	//Create the agent and the Database
	Agent * newCell = new Agent();
	Hive::Database *db = newCell->getDatabase();
	// //////////////////////////////////////////////////////////
	// Create the base Data objects that are needed by the cell
	/////////////////////////////////////////////////////////////
	//INPUTS
	IntegerData *division = new IntegerData("division",0.0);
	db->addData(division->getName(),division);
	DoubleData *biomass = new DoubleData("biomass",1.0);
	db->addData(biomass->getName(),biomass);
	DoubleData *maxBiomass = new DoubleData("maxBiomass",2.0);
	db->addData(maxBiomass->getName(),maxBiomass);
	DoubleData *vmax = new DoubleData("vmax",1/3600);
	db->addData(vmax->getName(),vmax);
	DoubleData *ks = new DoubleData("ks",0.001);
	db->addData(ks->getName(),ks);
	DoubleData *substrateConcentration = new DoubleData("substrateConcentration",0.001);
	db->addData(substrateConcentration->getName(),substrateConcentration);
	//OUTPUTS
	DoubleData *growthRate = new DoubleData("growthRate",0.0);
	db->addData(growthRate->getName(),growthRate);
	DoubleData *substrateUptake = new DoubleData("substrateUptake",0.0);
	db->addData(growthRate->getName(),substrateUptake);
	//ADDING SIMULATORS,ACTIONS,MESSAGE GENERATORS
	this->addSimulatorToAgent(newCell);
	this->addActionsToAgentsActionSet(newCell);
	this->addMessageGeneratorsToAgent(newCell);
	return newCell;
}

//WHY IS THIS HERE?
Agent* CellAgentFactory::createAgent(Action **as) {
	return NULL;
}

//WHY IS THIS HERE?
Agent* CellAgentFactory::duplicateAgent(Agent *ag) {
	return NULL;
}

//WHY IS THIS HERE?
void CellAgentFactory::finalise() {}

void CellAgentFactory::addActionsToAgentsActionSet(Agent *ag) {
	UpdateCellSeenLigandAction *ucsla = new UpdateCellSeenLigandAction();
	ucsla->setAgent(ag);
	ag->addAction(ucsla);
}

void CellAgentFactory::addMessageGeneratorsToAgent(Agent *ag) {
	CellNotifyWorldOfUptake *cnwu = new CellNotifyWorldOfUptake(ag);
	ag->addMessageGenerator(cnwu);
	CellNotifyWorldOfDivision *cnwd = new CellNotifyWorldOfDivision(ag);
	ag->addMessageGenerator(cnwd);
}

void CellAgentFactory::addSimulatorToAgent(Agent *ag) {
	// Add the division simulator
	Division *divsim = new Division();
	divsim->setLinksToAgentDB("biomassIndex","substrateConcentrationArray","maxBiomass","division","cellVolume");
	divsim->setAgent(ag);
	ag->addSimulator(divsim);
	// Add the growth/uptake simulator
	MichealisMenten *sim = new MichealisMenten();
	sim->setLinksToAgentDB("ksArray","vmaxArray","mmSubstrateIndecies","substrateConcentrationArray","mmReactionIndecies","rateArray");
	sim->setAgent(ag);
	ag->addSimulator(sim);
	//Add the FBA simulator
	FBA *sim = new FBA();
	String lowerBound("rateArray");
	String upperBound("rateArray");
	sim->setLinksToAgentDB("boundaryIndecies","boundaryLowerBound","boundaryUpperBound","mmReactionIndecies",lowerBound,upperBound,"rateArray","boundaryArray","growthRate");
	sim->setAgent(ag);
	ag->addSimulator(sim);
	//Add mass balance simulator
	MassBalance *sim = new MassBalance();
	sim->setLinksToAgentDB("boundaryIndecies","boundaryArray","substrateConcentrationArray","cellVolume");
	sim->setAgent(ag);
	ag->addSimulator(sim);
}
