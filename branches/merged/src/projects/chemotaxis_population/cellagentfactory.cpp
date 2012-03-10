#include "cellagentfactory.hh"

using namespace std;
using namespace Hive;
using namespace ChemoPop;


CellAgentFactory::CellAgentFactory()  { }

CellAgentFactory::CellAgentFactory(InputSystemReader *isr) { }

CellAgentFactory::~CellAgentFactory() { }


Agent* CellAgentFactory::createAgent()
{
	//Create the agent and the Database
	Agent *bond = new Agent();
	Hive::Database *db = bond->getDatabase();

	// //////////////////////////////////////////////////////////
	// Create the base Data objects that are needed by the cell
	/////////////////////////////////////////////////////////////

	// Internal clock
	DoubleData *celltime = new DoubleData("celltime", 0.0);
	db->addData(celltime->getName(), celltime);

	// Equilibration Time
	DoubleData *eqtime = new DoubleData("eqtime", cpi->getEqTime());
	db->addData(eqtime->getName(), eqtime);

	// Output frequency
	DoubleData *outputinterval_Data = new DoubleData("outputinterval",this->output_interval);
	db->addData(outputinterval_Data->getName(), outputinterval_Data);

	// Swimming state of the cell
	IntegerData *swimming_state = new IntegerData("swimming_state",0);
	db->addData(swimming_state->getName(), swimming_state);
	IntegerData *last_swimming_state = new IntegerData("last_swimming_state",0);
	db->addData(last_swimming_state->getName(), last_swimming_state);

	// at the present the world can only handle one ligand profile ...
	TVectorData<double> *ligands = new TVectorData<double> ("ligands","tvector_double");
	ligands->reserveSize(1);
	ligands->at(0) = 0;
	db->addData(ligands->getName(),ligands);

	// this stores how much  nutrient a cell would like to get from the world
	TVectorData<double> *appetite = new TVectorData<double> ("appetite", "tvector_double");
	appetite->reserveSize(1);
	appetite->at(0) = 0;
	db->addData(appetite->getName(), appetite);

	// this stores the rates with which a cell consume nutrients
	TVectorData<double> *consumption_rates = new TVectorData<double> ("consumption_rates", "tvector_double");
	consumption_rates->reserveSize(1);
	consumption_rates->at(0) = 0.1;
	db->addData(consumption_rates->getName(), consumption_rates);

	// this stores the total accumulated ligand
	TVectorData<double> *accumulated_ligands = new TVectorData<double> ("accumulated_ligands","tvector_double");
	accumulated_ligands->reserveSize(1);
	accumulated_ligands->at(0) = 0;
	db->addData(accumulated_ligands->getName(),accumulated_ligands);

	/// cost for dividing
	DoubleData *hooker_cost  = new DoubleData("hooker_cost", 0.3);
	db->addData(hooker_cost->getName(), hooker_cost);

	// costs for movement
	DoubleData *cost_for_movement = new DoubleData("cost_for_movement", 0.3);
	db->addData(cost_for_movement->getName(), cost_for_movement);

	// cost for living
	DoubleData *cost_for_living   = new DoubleData("cost_for_living", 0.1);
	db->addData(cost_for_living->getName(), cost_for_living);

	/// threshold for dying
	DoubleData *die_threshold  = new DoubleData("die_threshold", 0.4);
	db->addData("die_threshold", die_threshold);

	/// flag that will be set by the death simulator, if the cell has to die
	BoolData *death_flag = new BoolData("death_flag", false);
	db->addData(death_flag->getName(), death_flag);

	/// flag that will be set by the birth simulator, if the cell gives rise to offspring
	BoolData *birth_flag = new BoolData("birth_flag", false);
	db->addData(birth_flag->getName(), birth_flag);

	/// flag that get sets if the cell decides to move
	BoolData *move_flag = new BoolData("move_flag", false);
	db->addData(move_flag->getName(), move_flag);

	/// probabilities according to which we will choose what an agent does in a single step
	TVectorData<double> *what_to_do_probabilities = new TVectorData<double> ("what_to_do_probabilities", "tvector_double");
	what_to_do_probabilities->reserveSize(2);
	// the first will be the probability to move the second to reproduce
	what_to_do_probabilities->at(0) = 0.8;
	what_to_do_probabilities->at(1) = 0.1;
	db->addData(what_to_do_probabilities->getName(), what_to_do_probabilities);

	// ///////////////////////////////////////////////////////////////////
	// Initialize (also inserts) additional Data into the database
	this->cpi->setNextCellParameters(bond);

	// ///////////////////////////////////////////////////////////////////
	// add simulators to agent
	this->addSimulatorToAgent(bond);

	// add actions to agents action set
	this->addActionsToAgentsActionSet(bond);

	// add message generators to action set
	this->addMessageGeneratorsToAgent(bond);


	return bond;
}




Agent* CellAgentFactory::createAgent(Action **as) {
	Agent *bond = new Agent();
	return bond;
}

Agent* CellAgentFactory::duplicateAgent(Agent *ag) {
	Agent *bond = new Agent();
	return bond;
}

void CellAgentFactory::finalise() {
}


void CellAgentFactory::addActionsToAgentsActionSet(Agent *ag) {
	UpdateCellSeenLigandAction *ucsla = new UpdateCellSeenLigandAction();
	ucsla->setAgent(ag);
	ag->addAction(ucsla);
}


void CellAgentFactory::addMessageGeneratorsToAgent(Agent *ag) {
	CellNotifyWorldAboutSwimmingStateMessageGenerator *cnwassmg = new CellNotifyWorldAboutSwimmingStateMessageGenerator(ag) ;
	ag->addMessageGenerator(cnwassmg);
	CellRevealItselfToSpecialAgentMessageGenerator *critsamg = new CellRevealItselfToSpecialAgentMessageGenerator(ag);
	ag->addMessageGenerator(critsamg);
}


void CellAgentFactory::addSimulatorToAgent(Agent *ag)
{
	// Add the simple cell simulator
	SimpleSignalingSimulator *sim = new SimpleSignalingSimulator();
	sim->setAgent(ag);
	ag->addSimulator(sim);

	// Add the basic motor simulator
	MotorSimulator *motSim = new MotorSimulator();
	motSim->setAgent(ag);
	ag->addSimulator(motSim);


	// Check what kind of flagella model was loaded
	int flagellaID = ((IntegerData *)(ag->getDatabase()->getDataItem("flagellaModelType")))->getInteger();
	if(flagellaID==CellParameterData::FLAGELLA_VOTING_MODEL) {
		VotingModelSimulator *voteSim = new VotingModelSimulator();
		voteSim->setAgent(ag);
		ag->addSimulator(voteSim);
	} else if (flagellaID==CellParameterData::FLAGELLA_CONFORMATION_MODEL) {
		ConformationModelSimulator *flagSim = new ConformationModelSimulator();
		flagSim->setAgent(ag);
		ag->addSimulator(flagSim);

	} else if (flagellaID==CellParameterData::FLAGELLA_SOURJICK_MODEL) {
		cerr<<"SOURJICK FLAGELLA MODEL NOT YET IMPLEMENTED!!!"<<endl;
		exit(1);
	} else {
		cerr<<"INVALID FLAGELLA MODEL SELECTED!!!"<<endl;
		exit(1);
	}

	// add simplest cell metabolism simulator to cell
	SimplestCellMetabolismSimulator *scms = new SimplestCellMetabolismSimulator(ag);
	scms->setAgent(ag);
	ag->addSimulator(scms);

	/// add time keeper
	TimeKeeper *ctk = new TimeKeeper(ag,"celltime");
	ag->addSimulator(ctk);

}
