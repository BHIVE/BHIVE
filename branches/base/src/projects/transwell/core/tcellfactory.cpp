/*
 * tcellfactory.cpp
 *
 *  Created on: Dec 15, 2009
 *      Author: jentsch
 */

#include "tcellfactory.hh"

using namespace Transwell;

TCellAgentFactory::TCellAgentFactory() {
	this->two_cell_types = false;
	cerr << "# hello, i am the transwell's tcell agent-factory" << endl;
}

TCellAgentFactory::~TCellAgentFactory() {
	cerr << "# killing the TCellAgentFactory" << endl;
	delete gsr;
	delete gsrII;
	cerr << "# done killing the TCellAgentFactory" << endl;
}

void TCellAgentFactory::setCellNumbers(int eff, int reg) {
	this->two_cell_types = true;
	this->number_of_teff_cells = eff;
	this->number_of_treg_cells = reg;
	this->number_of_cells_created = 0;
}

Agent* TCellAgentFactory::createAgent() {
  //  cerr << this->number_of_teff_cells << endl; 
  //  cerr << this->number_of_cells_created << endl;
  //  exit(1);
	if (this->two_cell_types == true) {
	  this->number_of_cells_created += 1;
		if (this->number_of_cells_created <= this->number_of_teff_cells)
			return this->createTeffCell();
		else
			return this->createTregCell();
	} else {
		return this->createTeffCell();
	}
}

Agent* TCellAgentFactory::createTregCell() {
	Agent *tcell = new Agent();

	Hive::Database *db = tcell->getDatabase();

	// make a copy of the systems rate vector that will be passed to the agent's database
	TVectorData<GRate* > *r = new TVectorData<GRate* >("rate_vector", "tvectordata_double");
	r->reserveSize(gsysII->getPtrToRatesVector()->size());
	for (int i=0; i<r->size(); i++) {
		GRate *rate = new GRate("rate","grateptr");
		rate->copy(gsysII->getPtrToRatesVector()->at(i));
		r->at(i) = rate;
	}
	TVectorData<TVectorData<int> > *stoch = new TVectorData<TVectorData<int> > ();
	stoch->reserveSize(gsysII->getPtrToStoichiometryMatrix()->size());
	for (int i=0; i<stoch->size(); i++) {
		stoch->at(i).reserveSize(gsysII->getPtrToStoichiometryMatrix()->at(i).size());
		for (int j=0; j<stoch->at(i).size(); j++)
			stoch->at(i).setEntry(j,gsysII->getPtrToStoichiometryMatrix()->at(i).at(j));
	}
	// check whether the correct names are given
	// NOTE THAT WE ARE ALWAYS PASSING A POINTER TO THE SAME OBJECT TO THE CELL!!!!
	db->addData("rate_vector", r);
	db->addData("stoich_matrix", stoch);
	TVectorData<double> *conc = new TVectorData<double> ("concentrations", "tvectordata_double");
	conc->reserveSize(gsysII->getPtrToConcentrationNames()->size());
	// il2
	conc->at(0) = 0;
	// reca
	conc->at(1) = 10000;
	// recb
	conc->at(2) = 300;
	// stat5
	conc->at(5) = 2000;
	// chrom_il2
	conc->at(7) = 0;
	// chrom_reca
	conc->at(10) = 2;
	// chrom_recb
	conc->at(12) = 2;

	// chrom will be initialised via messages from nucleus!
	db->addData("concentrations", conc);

	// tcell agent needs to know in which position they live
	TVectorData<double> *mypos = new TVectorData<double>("position","tvectordata_double");
	mypos->reserveSize(3);

	// <--------------- BEGIN DUMMY INITIALISATION OF POSITION FOR TESTING THE HYBRID SIMULATOR

	mypos->at(0) = 0.1;
	mypos->at(1) = 0.1;
	mypos->at(2) = 0.5;

	// add position data to database
	db->addData("position", mypos);

	DoubleData *time = new DoubleData("time", 0.0);
	DoubleData *propagate_until = new DoubleData("propagate_until",0.0);
	DoubleData *loc_prop_until = new DoubleData("local_propagate_until",0.0);
	BoolData *event_occured = new BoolData("event_occured", false);

	db->addData("time", time);
	db->addData("propagate_until", propagate_until);
	db->addData("local_propagate_until",loc_prop_until);
	db->addData("event_occured", event_occured);

	TVectorData<double> *remember = new TVectorData<double>("remember", "tvectordata_double");
	remember->reserveSize(0);
	db->addData("remember",remember);

	IntegerData *number_between_outputs = new IntegerData("number_between_outputs",216);
	IntegerData *counter                = new IntegerData("counter", 216);
	db->addData("number_between_outputs", number_between_outputs);
	db->addData("counter", counter);

	// add simulator to world agent
	this->addSimulatorToAgent(tcell);
	this->addActionsToAgentsActionSet(tcell);
	this->addMessageGeneratorsToTregCell(tcell);

	return tcell;
}

Agent* TCellAgentFactory::createTeffCell() {
		Agent *tcell = new Agent();

		// get the database of the tcell
		Hive::Database *db = tcell->getDatabase();

		// make a copy of the systems rate vector that will be passed to the agent's database
		TVectorData<GRate* > *r = new TVectorData<GRate* >("rate_vector", "tvectordata_double");
		r->reserveSize(gsys->getPtrToRatesVector()->size());
		for (int i=0; i<r->size(); i++) {
			GRate *rate = new GRate("rate","grateptr");
			rate->copy(gsys->getPtrToRatesVector()->at(i));
			r->at(i) = rate;
		}
		TVectorData<TVectorData<int> > *stoch = new TVectorData<TVectorData<int> > ();
		stoch->reserveSize(gsys->getPtrToStoichiometryMatrix()->size());
		for (int i=0; i<stoch->size(); i++) {
			stoch->at(i).reserveSize(gsys->getPtrToStoichiometryMatrix()->at(i).size());
			for (int j=0; j<stoch->at(i).size(); j++)
				stoch->at(i).setEntry(j,gsys->getPtrToStoichiometryMatrix()->at(i).at(j));
		}

		// check whether the correct names are given
		// NOTE THAT WE ARE ALWAYS PASSING A POINTER TO THE SAME OBJECT TO THE CELL!!!!
		db->addData("rate_vector", r);
		db->addData("stoich_matrix", stoch);

		// provide vector for the chemical species that will live in the tcells
		// the name concentrations is a little misleading. we have number of particles. however
		// we treat them deterministically such that we have to use doubles.
		TVectorData<double> *conc = new TVectorData<double> ("concentrations", "tvectordata_double");
		conc->reserveSize(gsys->getPtrToConcentrationNames()->size());

		// here we initialise the concentrations, again we know where the species live
		conc->at(0) = 0;
		// reca
		conc->at(1) = 0;
		// recb
		conc->at(2) = 300;
		// stat5
		conc->at(5) = 2000;
		// chrom_il2
		conc->at(7) = 2;
		// chrom_reca
		conc->at(10) = 2;
		// chrom_recb
		conc->at(12) = 2;

		// chrom will be initialised via messages from nucleus!
		db->addData("concentrations", conc);

		// tcell agent needs to know in which position they live
		TVectorData<double> *mypos = new TVectorData<double>("position","tvectordata_double");
		mypos->reserveSize(3);

		// <--------------- BEGIN DUMMY INITIALISATION OF POSITION FOR TESTING THE HYBRID SIMULATOR

		mypos->at(0) = 0.1;
		mypos->at(1) = 0.1;
		mypos->at(2) = 0.001;

		// <---------------- END DUMMY INITIALISATION OF POSITION FOR TESTING THE HYBRID SIMULATOR

		// <-------------- BEGIN INITIALISATION OF POSITION FOR TESTING DISCRETISATION IN CUBE GEOMETRY
		/*
		mypos->at(0) = Util::RANDOM(1.0);
		mypos->at(1) = Util::RANDOM(1.0);
		mypos->at(2) = 0.0001;
		*/
		// <---------------- END INITIALISATION OF POSITION FOR TESTING DISCRETISATION IN CUBE GEOMETRY

		// <--------------- BEGIN INITIALISATION OF POSITION FOR TRANSWELL GEOEMETRY...
		/*
		double radius;
		double phi;
		do {
			radius = Util::RANDOM(8.0);
			phi = Util::RANDOM(360.0)*180./3.14159265;
			mypos->at(0) = radius*cos(phi);
			mypos->at(1) = radius*sin(phi);
		} while ((mypos->at(0)*mypos->at(0)+mypos->at(1)*mypos->at(1))>(7.8*7.8));
		mypos->at(2) = 0.0001;
		 */

		// <---------------- END INITIALISATION OF POSITIONS FOR TRANSWELL GEOMETRY.


		// add position data to database
		db->addData("position", mypos);

		DoubleData *time = new DoubleData("time", 0.0);
		DoubleData *propagate_until = new DoubleData("propagate_until",0.0);
		DoubleData *loc_prop_until = new DoubleData("local_propagate_until",0.0);
		BoolData *event_occured = new BoolData("event_occured", false);

		db->addData("time", time);
		db->addData("propagate_until", propagate_until);
		db->addData("local_propagate_until",loc_prop_until);
		db->addData("event_occured", event_occured);

		TVectorData<double> *remember = new TVectorData<double>("remember", "tvectordata_double");
		remember->reserveSize(0);
		db->addData("remember",remember);

		IntegerData *number_between_outputs = new IntegerData("number_between_outputs",216);
		IntegerData *counter                = new IntegerData("counter", 216);
		db->addData("number_between_outputs", number_between_outputs);
		db->addData("counter", counter);

		// add simulator to world agent
		this->addSimulatorToAgent(tcell);
		this->addActionsToAgentsActionSet(tcell);
		this->addMessageGeneratorsToAgent(tcell);


		return tcell;
}

Agent* TCellAgentFactory::createAgent(Action **as) {
	cerr << "in TCellFactory::createAgent(Agent *ag): creating dummy agent without any functionality" << endl;

	Agent *tcell = new Agent();

	return tcell;
}

Agent* TCellAgentFactory::duplicateAgent(Agent *ag) {
	return NULL;
}


void TCellAgentFactory::finalise() {
	cerr << "# finalising the TCellAgentFactory .... " << endl;

	// this is for teff cells
	gsr = new GSystemReader();
	ifstream input("test/testtranswellsystem.txt");
	this->gsys = (GSystem*) gsr->parseSystem(input);

	// this is for the treg_cells
	gsrII = new GSystemReader();
	ifstream inputII("test/testtranswellsystemII.txt");
	this->gsysII = (GSystem*) gsrII->parseSystem(inputII);

//	cerr << "# generating output for manually validating the parsed system: " << endl;
//	this->gsys->printContent(cerr);
	cerr<< "# done " << endl;
}

void TCellAgentFactory::addActionsToAgentsActionSet(Agent *ag) {
	Action *a1 = new UpdateCellSeenIL2Action();
	a1->setAgent(ag);
	Action *a2 = new UpdateCellSeenParticlesAction();
	a2->setAgent(ag);

	ag->addAction(a1);
	ag->addAction(a2);
}

void TCellAgentFactory::addMessageGeneratorsToAgent(Agent *ag) {
	// message generator for initialisation
	CellNotifyWorldAboutPositionForInitMessageGenerator *cnwpfimg = new CellNotifyWorldAboutPositionForInitMessageGenerator(ag);
	ag->addMessageGeneratorForInitialisation(cnwpfimg);

	// normal message generators
	CellNotifyPSTAT5ToNucleusMessageGenerator *cnptnmg = new CellNotifyPSTAT5ToNucleusMessageGenerator(ag);
	ag->addMessageGenerator(cnptnmg);
	CellNotifyIL2ToWorldMessageGenerator *cnitwmg = new CellNotifyIL2ToWorldMessageGenerator(ag);
	ag->addMessageGenerator(cnitwmg);
	CellRevealItselfToSpecialAgentMessageGenerator *critsamg = new CellRevealItselfToSpecialAgentMessageGenerator(ag);
	ag->addMessageGenerator(critsamg);
}


void TCellAgentFactory::addMessageGeneratorsToTregCell(Agent *ag) {
	// message generator for initialisation
	TregNotifyWorldAboutPositionForInitMessageGenerator *tnwpfimg = new TregNotifyWorldAboutPositionForInitMessageGenerator(ag);
	ag->addMessageGeneratorForInitialisation(tnwpfimg);

	// normal message generators
	CellNotifyPSTAT5ToNucleusMessageGenerator *cnptnmg = new CellNotifyPSTAT5ToNucleusMessageGenerator(ag);
	ag->addMessageGenerator(cnptnmg);
	CellNotifyIL2ToWorldMessageGenerator *cnitwmg = new CellNotifyIL2ToWorldMessageGenerator(ag);
	ag->addMessageGenerator(cnitwmg);
	TregRevealItselfToSpecialAgentMessageGenerator *tritsamg = new TregRevealItselfToSpecialAgentMessageGenerator(ag);
	ag->addMessageGenerator(tritsamg);

}

void TCellAgentFactory::addSimulatorToAgent(Agent *ag) {
	cerr << "# adding Simulator to agent ..." << endl;
	GRKOde *runge = new GRKOde();

	//  uncomment for testing the runge kutta solver
	//	runge->testing();
	//	exit(1);

	runge->setTimeStep(1.0);
	runge->setAgent(ag);

	ag->addSimulator(runge);

	// runge->generateOutputForValidation();
	// runge->testing();

	cerr << "# ... done " << endl;

}

