/*
 * tcellfactory.cpp
 *
 *  Created on: Dec 15, 2009
 *      Author: jentsch
 */

#include "tcellfactory.hh"

using namespace Transwell;

TCellAgentFactory::TCellAgentFactory() {
	cerr << "# hello, i am the transwell's tcell agent-factory" << endl;
}

TCellAgentFactory::TCellAgentFactory(InputSystemReader *isr):AgentFactory(isr) {
	cerr << "# hello, i am the transwell's tcell agent-factory" << endl;
}

TCellAgentFactory::~TCellAgentFactory() {
	cerr << "# killing the TCellAgentFactory" << endl;
	delete gsr;
	cerr << "# done killing the TCellAgentFactory" << endl;
}

Agent* TCellAgentFactory::createAgent() {
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
		// here we initialise the concentrations, again we know where the
		// il2
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
		// initialisation
		mypos->at(0) = 0.1;
		mypos->at(1) = 0.1;
		mypos->at(2) = 0.001;
		db->addData("position", mypos);

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
	gsr = new GSystemReader();
	ifstream input("test/testtranswellsystem.txt");
	this->gsys = (GSystem*) gsr->parseSystem(input);
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

