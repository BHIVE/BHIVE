/*
 * nucleusagentfactory.cpp
 *
 * Created on: Jan 12, 2010
 *     Author: jentsch
 */


#include "nucleusagentfactory.hh"

using namespace Transwell;

NucleusAgentFactory::NucleusAgentFactory() {
	cerr << "# hello, i am the transwell's nucleus agent-factory" << endl;
}

NucleusAgentFactory::~NucleusAgentFactory() { delete gsr; }

Agent* NucleusAgentFactory::createAgent() {
		Agent *nucleus = new Agent();

		// get the database of the nucleus
		Hive::Database *db = nucleus->getDatabase();

		// adding data to the database
		// to add the reaction rates, we need to make a copy of
		// the systems rate vector
		TVectorData<GRate* > *r = new TVectorData<GRate* >("rate_vector", "tvectordata_double");
		r->reserveSize(gsys->getPtrToRatesVector()->size());
		for (int i=0; i<r->size(); i++) {
			GRate *rate = new GRate("rate","grateptr");
			rate->copy(gsys->getPtrToRatesVector()->at(i));
			r->at(i) = rate;
		}
		// add the rate vector
		db->addData("rate_vector", r);
		// the particle numbers
		TVectorData<int> *particle_numbers = new TVectorData<int> ("particle_numbers", "tvectordata_int");
		particle_numbers->reserveSize(gsys->getPtrToConcentrationNames()->size());
		// INITIALISATION: again we know were which particles are stored.
		// chrom_il2
		particle_numbers->at(2) = 2;
		// chrom_reca
		particle_numbers->at(5) = 2;
		// chrom_recb
		particle_numbers->at(8) = 2;
		db->addData("particle_numbers", particle_numbers);
		// the propensities
		TVectorData<double> *propensities = new TVectorData<double> ("propensities", "tvectordata_double");
		propensities->reserveSize(gsys->getPtrToRatesVector()->size());
		db->addData("propensities", propensities);
		// the total propensity
		DoubleData *total_prop = new DoubleData("total_propensites", 0);
		db->addData("total_propensity", total_prop);
		// volume
		DoubleData *volume = new DoubleData("volume", 1);
		db->addData("volume", volume);
		// time
		DoubleData *time = new DoubleData("time",0.0);
		db->addData("time", time);
		// next reaction
		DoubleData *next_reac_time = new DoubleData("next_reac_time",0.0);
		db->addData("next_reac_time", next_reac_time);

		// time until which it has to propagate
		DoubleData *propagate_until = new DoubleData("propagate_until", 0.0);
		db->addData("propagate_until", propagate_until);

		BoolData *generate_message = new BoolData("generate_message", false);
		db->addData("generate_message", generate_message);

		BoolData *recalculate_completely = new BoolData("recalculate_completely", true);
		db->addData("recalculate_compeletely", recalculate_completely);

		DoubleData *delta = new DoubleData("delta",0.0);
		db->addData("delta", delta);

		this->addActionsToAgentsActionSet(nucleus);
		this->addMessageGeneratorsToAgent(nucleus);
		this->addSimulatorToAgent(nucleus);

		return nucleus;
}

Agent* NucleusAgentFactory::createAgent(bool is_treg_cell) {
  if(!is_treg_cell) {
    return this->createAgent();
  } else {
    Agent *nuc = this->createAgent();
    TVectorData<int> *part = (TVectorData<int> *) nuc->getDatabase()->getDataItem("particle_numbers");
    part->at(2) = 0;
    return nuc;
  }
}

Agent* NucleusAgentFactory::createAgent(Action **as) {
	cerr << "in NucleusAgentFactory::createAgent(Action *as) creating dummy agent without any functionality" << endl;

	Agent *nucleus = new Agent();

	return nucleus;
}

Agent* NucleusAgentFactory::duplicateAgent(Agent *ag) {
	return NULL;
}

void NucleusAgentFactory::finalise() {
	cerr << "# finalising the NucleusAgentFactory ... " << endl;
	gsr = new GSystemReader();
	ifstream input("test/testnucleus.txt");
	this->gsys = (GSystem*) gsr->parseSystem(input);
//	cerr << "# outputting parsed system so that you can validate it: " << endl;
//	this->gsys->printContent(cerr);
	cerr << "# done" << endl;
}

void NucleusAgentFactory::addActionsToAgentsActionSet(Agent *ag) {
	Action *a1 = new UpdateNucleusSeenPSTAT5Action();
	a1->setAgent(ag);
	ag->addAction(a1);
}

void NucleusAgentFactory::addMessageGeneratorsToAgent(Agent *ag) {
	NucleusNotifyParticleNumbersToCellMessageGenerator *nnpntcmg = new NucleusNotifyParticleNumbersToCellMessageGenerator(ag);
	ag->addMessageGenerator(nnpntcmg);
}

void NucleusAgentFactory::addSimulatorToAgent(Agent *ag) {
	cerr << "# adding Simulator to agent ... " << endl;
	DirectSSA *gillespie = new DirectSSA();

	// for testing the algorithm uncomment these two lines
	//	gillespie->testing();
	//	exit(1);

	gillespie->setAgent(ag);
	ag->addSimulator(gillespie);

	cerr << "# ... done" << endl;

}

