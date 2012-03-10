#include "cellagentfactory.hh"

using namespace std;
using namespace Hive;
using namespace ChemoPop;


CellAgentFactory::CellAgentFactory()  {
	this->generate_cell_data_output = false;
}

CellAgentFactory::~CellAgentFactory() { }

Agent* CellAgentFactory::createAgent()
{
	//cerr<<"CellAgentFactory::createAgent()"<<endl;

	//Create the agent and the Database
	Agent *bond = new Agent();
	Hive::Database *db = bond->getDatabase();

	// //////////////////////////////////////////////////////////
	// Create the base Data objects that are needed by the cell
	/////////////////////////////////////////////////////////////

	// Internal clock
	DoubleData *celltime = new DoubleData("celltime", 0.0);
	db->addData(celltime->getName(), celltime);

	// Internal clock
	DoubleData *dt = new DoubleData("dt", 0.01);
	db->addData(dt);

	// Equilibration Time
	DoubleData *eqtime = new DoubleData("eqtime", this->equilibration_time);
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

	// If you change this, be sure to change the cell_acc_ligands in the cellparameterinitializer!!
	ligands->reserveSize(1);
	ligands->at(0) = 0;
	db->addData(ligands->getName(),ligands);


	// this stores the total accumulated ligand
	TVectorData<double> *accumulated_ligands = new TVectorData<double> ("accumulated_ligands","tvector_double");
	accumulated_ligands->reserveSize(1);
	accumulated_ligands->at(0) = 0;
	db->addData(accumulated_ligands->getName(),accumulated_ligands);



	// here i am adding two double data items to the database for rememberbing the aktivitaet
	// and the amount of cheyphos. i need that. nothing as a good as an ugly piece of code.
	// but i won't tell anyone. hehe.
	DoubleData *frank = new DoubleData("aktivitaet",0.0);
	db->addData(frank);
	DoubleData *sascha = new DoubleData("cheyphos", 0.0);
	db->addData(sascha);
	DoubleData *jochim = new DoubleData("methyl", 0.0);
	db->addData(jochim);

	// here are yet some more methods for tracking the change in activity, yp, ligand
	// since the last timestep
	DoubleData *dLig = new DoubleData("dLigand",0.0);
	db->addData(dLig);
	DoubleData *dCheYP = new DoubleData("dCheYP", 0.0);
	db->addData(dCheYP);
	DoubleData *dActivity = new DoubleData("dActivity", 0.0);
	db->addData(dActivity);
	DoubleData *dMeth = new DoubleData("dMeth", 0.0);
	db->addData(dMeth);




	double startTimeForHistory = this->equilibration_time+30; double endTimeForHistory = 9999999;
	double historyLengthInSeconds = 60;
	int historyLength = (int)(historyLengthInSeconds/dt->getDouble());

	HistoryCollector *hcLig = new HistoryCollector("ligHistory", historyLength);
	db->addData(hcLig);
	HistoryAggregator *haLig_TUMBLE = new HistoryAggregator("ligAggregator", hcLig, startTimeForHistory, endTimeForHistory);
	db->addData(haLig_TUMBLE);

	HistoryCollector *hcdlogLig = new HistoryCollector("dlogligHistory", historyLength);
	db->addData(hcdlogLig);
	HistoryAggregator *hadlogLig_TUMBLE = new HistoryAggregator("dlogligAggregator", hcdlogLig, startTimeForHistory, endTimeForHistory);
	db->addData(hadlogLig_TUMBLE);

	HistoryCollector *hcCheYP = new HistoryCollector("cheYpHistory", historyLength);
	db->addData(hcCheYP);
	HistoryAggregator *haCheYP_TUMBLE = new HistoryAggregator("cheYpTumbleAggregator", hcCheYP, startTimeForHistory, endTimeForHistory);
	db->addData(haCheYP_TUMBLE);

	HistoryCollector *hcAct = new HistoryCollector("actHistory", historyLength);
	db->addData(hcAct);
	HistoryAggregator *haAct_TUMBLE = new HistoryAggregator("actTumbleAggregator", hcAct, startTimeForHistory, endTimeForHistory);
	db->addData(haAct_TUMBLE);

	HistoryCollector *hcMeth = new HistoryCollector("methHistory", historyLength);
	db->addData(hcMeth);
	HistoryAggregator *haMeth_TUMBLE = new HistoryAggregator("methTumbleAggregator", hcMeth, startTimeForHistory, endTimeForHistory);
	db->addData(haMeth_TUMBLE);

	HistoryCollector *hcMotorCW = new HistoryCollector("motorCWHistory", historyLength);
	db->addData(hcMotorCW);
	HistoryAggregator *haMotor_TUMBLE = new HistoryAggregator("motorTumbleAggregator", hcMotorCW, startTimeForHistory, endTimeForHistory);
	db->addData(haMotor_TUMBLE);

	HistoryCollector *hcRun = new HistoryCollector("runHistory", historyLength);
	db->addData(hcRun);
	HistoryAggregator *haRunHistory_TUMBLE = new HistoryAggregator("runHistoryTumbleAggregator", hcRun, startTimeForHistory, endTimeForHistory);
	db->addData(haRunHistory_TUMBLE);




	// ///////////////////////////////////////////////////////////////////
	// Initialize (also inserts) additional Data into the database
//	cerr << "calling cpi" << endl;
	this->cpi->setNextCellParameters(bond);
//	cerr << "done calling cpi" << endl;
	//cerr<<"adding simulators"<<endl;

	// ///////////////////////////////////////////////////////////////////
	// add simulators to agent
	this->addSimulatorToAgent(bond);

//	cerr<<"adding actions"<<endl;

	// add actions to agents action set
	this->addActionsToAgentsActionSet(bond);


//	cerr<<"adding mgs"<<endl;

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

	CellTellSpecialAgentAboutSeenLigandMessageGenerator *ctsaaslmg = new CellTellSpecialAgentAboutSeenLigandMessageGenerator(ag);
	ag->addMessageGenerator(ctsaaslmg);

	CellTellSpecialAgentAboutActivityMessageGenerator *ctsaaamg = new CellTellSpecialAgentAboutActivityMessageGenerator(ag);
	ag->addMessageGenerator(ctsaaamg);

	if (this->generate_cell_data_output) {
		CellRevealItselfToSpecialAgentMessageGenerator *critsamg = new CellRevealItselfToSpecialAgentMessageGenerator(ag);
		ag->addMessageGenerator(critsamg);
	}
}


void CellAgentFactory::addSimulatorToAgent(Agent *ag)
{
	// Add the simple cell simulator
	if(((BoolData *)ag->getDatabase()->getDataItem("is_FULL_MODEL"))->getBool()) {
		FullSignalingSimulator *sim = new FullSignalingSimulator();
		sim->setAgent(ag);
		ag->addSimulator(sim);
	} else {
		SimpleSignalingSimulator *sim = new SimpleSignalingSimulator();
		sim->setAgent(ag);
		ag->addSimulator(sim);
	}

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

	/// add time keeper
	TimeKeeper *ctk = new TimeKeeper(ag,"celltime");
	ag->addSimulator(ctk);
}
