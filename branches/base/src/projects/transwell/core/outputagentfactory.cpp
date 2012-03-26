#include "outputagentfactory.hh"

using namespace Transwell;


OutputAgentFactory::OutputAgentFactory():AgentFactory() {
	cerr << "# hello, i am the transwell's OutputAgentFactory" << endl;
	this->numbercells = 0;
	this->two_cell_types = false;
}

OutputAgentFactory::~OutputAgentFactory() {}

Agent* OutputAgentFactory::createAgent() {
	SpecialAgent *fox_mulder = new SpecialAgent();

	// meta database. i will store the meta information that might
	// be needed in a meta database of the special_Agent.
	Hive::Database *meta = fox_mulder->getMetaDatabase();

	// total_number_of_cells is being used by the output action SpecialAgentAccumulateDataAction
	if (!this->two_cell_types) {
		IntegerData *total_number_of_cells  = new IntegerData("total_number_of_cells", this->numbercells);
		meta->addData("total_number_of_cells", total_number_of_cells);
	} else {
		IntegerData *total_number_of_cells = new IntegerData("total_number_of_cells", this->number_teffs);
		meta->addData("total_number_of_cells", total_number_of_cells);
		IntegerData *total_number_tregs    = new IntegerData("total_number_of_tregs", this->number_tregs);
		meta->addData("total_number_tregs", total_number_tregs);
	}
	IntegerData *number_of_adds = new IntegerData("number_of_adds",0);
	meta->addData("number_of_adds", number_of_adds);
	DoubleData *global_time = new DoubleData("global_time", 0.0);
	meta->addData("global_time", global_time);
	DoubleData *time_for_treg = new DoubleData("time_for_treg", 0.0);
	meta->addData("time_for_treg", time_for_treg);
	DoubleData *time_step = new DoubleData("time_step", 216.0);
	meta->addData("time_step", time_step);
//	IntegerData *number_between_outputs = new IntegerData("number_between_outputs",216);
//	meta->addData("number_between_outputs", number_between_outputs);
//	IntegerData *counter = new IntegerData("counter", 215);
//	meta->addData("counter", counter);

	/// Data items for the one time only output of the PSTAT5 
	DoubleData *time_for_output = new DoubleData("time_to_do_single_output",59*3600.);
	BoolData *b1 = new BoolData("have_done_single_pstat5_output_for_t_cells",false);
	BoolData *b2 = new BoolData("have_done_single_pstat5_output_for_treg_cells",false);
	IntegerData *nafn = new IntegerData("number_of_adds_for_new",0);
	
	meta->addData(time_for_output);
	meta->addData(b1);
	meta->addData(b2);
	meta->addData(nafn);

	Hive::Database *db = fox_mulder->getDatabase();
	TVectorData<double> *total_conc = new TVectorData<double> ("total_conc", "tvectordata_double");
	total_conc->reserveSize(15);
	db->addData("total_conc", total_conc);

	TVectorData<double> *pfn = new TVectorData<double> ("pstat5_for_new", "tvectordata_double");
	if (!this->two_cell_types) 
	  pfn->reserveSize(this->numbercells);
	else
	  pfn->reserveSize(this->number_teffs);
	db->addData("pstat5_for_new",pfn);

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

void OutputAgentFactory::setNumberTeffsTregs(int teff, int treg) {
	this->two_cell_types = true;
	this->number_teffs   = teff;
	this->number_tregs   = treg;
}

void OutputAgentFactory::addActionsToAgentsActionSet(Agent *ag) {
	Action *a1 = new SpecialAgentAccumulateDataAction();
	((SpecialAgentAccumulateDataAction* ) a1)->prepareOutputfile("tcell_concentrations.txt");
	a1->setAgent(ag);
	ag->addAction(a1);
	Action *a2 = new SpecialAgentOutputGlobalIL2ConcentrationsAction();
	((SpecialAgentOutputGlobalIL2ConcentrationsAction* ) a2)->prepareOutputfile("il2_concentrations.txt");
	a2->setAgent(ag);
	ag->addAction(a2);
	Action *a3 = new SpecialAgentOutputIL2ProfileAction();
	a3->setAgent(ag);
	ag->addAction(a3);
	Action *a4 = new SpecialAgentAccumulateTregDataAction();
	((SpecialAgentAccumulateTregDataAction* ) a4)->prepareOutputfile("treg_concentrations.txt");
	a4->setAgent(ag);
	ag->addAction(a4);
}

void OutputAgentFactory::addSimulatorToAgent(Agent *ag) { }

void OutputAgentFactory::addMessageGeneratorsToAgent(Agent *ag) { }

