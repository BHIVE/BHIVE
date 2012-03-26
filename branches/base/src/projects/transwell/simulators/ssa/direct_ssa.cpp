#include "direct_ssa.hh"
#include <fstream>

using namespace std;
using namespace Hive;


DirectSSA::DirectSSA() {old_propensity == 0;}

DirectSSA::~DirectSSA() {}

void DirectSSA::setAgent(Agent *ag) {
	this->ag               = ag;
	this->reactions        = (TVectorData<GRate* > *) this->ag->getDatabase()->getDataItem("rate_vector");
	this->total_propensity = (DoubleData *) this->ag->getDatabase()->getDataItem("total_propensity");
	this->propensities     = (TVectorData<double> *) this->ag->getDatabase()->getDataItem("propensities");
	this->particle_numbers = (TVectorData<int> *) this->ag->getDatabase()->getDataItem("particle_numbers");
	this->agent_volume     = (DoubleData *) this->ag->getDatabase()->getDataItem("volume");
	this->time             = (DoubleData *) this->ag->getDatabase()->getDataItem("time");
	this->nxt              = (DoubleData *) this->ag->getDatabase()->getDataItem("next_reac_time");
	this->generate_message = (BoolData *) this->ag->getDatabase()->getDataItem("generate_message");
	this->recalculate_comp = (BoolData *) this->ag->getDatabase()->getDataItem("recalculate_compeletely");
}


void DirectSSA::singleStep(double t) {
	// get the initial time of this step
	double initial_time = this->time->getDouble();
	// local variable for storing and calculating the next reaction time
	double time;

	// if the last event has lead to a firing
	if (this->recalculate_comp->getBool() || old_propensity == 0) {
		time = initial_time - (1./(total_propensity->getDouble()))*log(1.-Util::RANDOM_CLOSED());
		this->nxt->setDouble(time);
		this->recalculate_comp->setBool(false);
	} else {
		// update time at which the next event will occur
		time = (old_propensity/total_propensity->getDouble())*(this->nxt->getDouble()-initial_time)+initial_time;
	}

	// if an event occurs within this time step
	if (time < ((DoubleData* )this->ag->getDatabase()->getDataItem("propagate_until"))->getDouble()) {
		// chose a reaction
		double cmp = total_propensity->getDouble() * Util::RANDOM_CLOSED();
		int index = 0;
		double weight = this->propensities->at(index);
		while (weight <= cmp) {
			weight += this->propensities->at(++index);
		}
		// we set the time at which the next reaction occurs to time
		// we need to do this, as the message generate will look for this time.
		this->nxt->setDouble(time);
		// fire the reaction
		this->reactions->at(index)->execute(this->particle_numbers);
		// tell agent to generate a message
		this->generate_message->setBool(true);
		// tell agent to recalculate the nextreactiontime from scratch the next time
		this->recalculate_comp->setBool(true);
	} else {                                       // if no event has occured,
		this->time->setDouble(initial_time+t);     // update the time of the agent to the end point of this time interval
		this->nxt->setDouble(time);	       		   // remember the nxt reaction time that lies in the future
	}
}

void DirectSSA::step(double t) {
	singleStep(t);
	/*
	// the argument is the time step
	double initial_time = this->time->getDouble();
	double end_time = initial_time + t;
	double cmp;
	double weight;
	int index = 0;

	if (this->nxt->getDouble() < 1000000000000) {
		initial_time = (old_propensity/total_propensity->getDouble())*(nxt->getDouble()-initial_time)+initial_time;
		if (initial_time < end_time) {
			// randomly choose a reaction by using the direct method
			index = 0;
			cmp = total_propensity->getDouble() * Util::RANDOM_CLOSED();
			weight = this->propensities->at(index);
			while (weight <= cmp) {
				weight += this->propensities->at(++index);
			}
			// update particle number
		//	cerr << "hier" << endl;
			this->reactions->at(index)->execute(this->particle_numbers);
		//	cerr << "done" << endl;
			// recalculate propensities. note, that we have not implemented a dependency graph as of yet.
			this->calcTotalPropensity();
			initial_time -= (1./(total_propensity->getDouble()))*log(1.-Util::RANDOM_CLOSED());
		}
	} else
		initial_time -= (1./(total_propensity->getDouble()))*log(1.-Util::RANDOM_CLOSED());
	// fire reactions as long as the local time is smaller than the end time
	while (initial_time < end_time) {
		// randomly choose a reaction by using the direct method
		index = 0;
		cmp = total_propensity->getDouble() * Util::RANDOM_CLOSED();
		weight = this->propensities->at(index);
		while (weight <= cmp) {
			weight += this->propensities->at(++index);
		}
		// update particle number
	//	cerr << "hier2" << endl;
		this->reactions->at(index)->execute(this->particle_numbers);
	//	cerr << "done2" << endl;
		// recalculate propensities. note, that we have not implemented a dependency graph as of yet.
		this->calcTotalPropensity();
		initial_time -= (1./(total_propensity->getDouble()))*log(1.-Util::RANDOM_CLOSED());
	}
	// remember the next reaction time
	this->nxt->setDouble(initial_time);
	// set time to the end of the time step.
	this->time->setDouble(end_time);
	*/
}

void DirectSSA::prepare() {
	// calculate the propensity of the reactions
	this->calcTotalPropensity();
	cerr << "# in DirectSSA::prepare() - implementation of dependency graph not yet implemented" << endl;
}

void DirectSSA::synchroniseWithDatabase() {
	// recalculate the propensity of the reactions
	this->old_propensity = total_propensity->getDouble();
	this->calcTotalPropensity();
}

// this testing routine simulates a little lotka volterra system
// Y_1 -> 2 Y_1 (10)
// Y_1 + Y_2 -> 2 Y_2 (0.01)
// Y_2 -> Z (10)
// Y_1(0) = Y_2(0) = 1000
void DirectSSA::testing() {
	cerr << "#entering the DirectSSA testing routine" << endl;

	// begin by setting up the relevant data structures
	this->reactions = new TVectorData<GRate* >("reactions", "tvectordata_grate");
	this->reactions->reserveSize(3);
	// first reaction
	GRate *g1 = new GRate("first_reac", "grate");
	g1->addEductSpecies(1,0);
	g1->addProductSpecies(2,0);
	g1->addRate(10.0);
	this->reactions->at(0) = g1;
	// second reaction
	GRate *g2 = new GRate("second_reac", "grate");
	g2->addEductSpecies(1,0);
	g2->addEductSpecies(1,1);
	g2->addProductSpecies(2,1);
	g2->addRate(0.01);
	this->reactions->at(1) = g2;
	// third reaction
	GRate *g3 = new GRate("third_reac", "grate");
	g3->addEductSpecies(1,1);
	g3->addProductSpecies(1,2);
	g3->addRate(10.0);
	this->reactions->at(2) = g3;

	// propensities
	this->total_propensity = new DoubleData("total_propensity", 0.0);
	this->propensities = new TVectorData<double> ("propensities", "tvector_double");
	this->propensities->reserveSize(3);

	// particle numbers
	this->particle_numbers = new TVectorData<int> ("particle_numbers", "tvector_int");
	this->particle_numbers->reserveSize(3);
	this->particle_numbers->at(0) = 1000;
	this->particle_numbers->at(1) = 1000;
	this->particle_numbers->at(2) = 0;

	// agent volume and time
	this->agent_volume = new DoubleData("volume", 1.0);
	this->time = new DoubleData("time", 0.0);

	// calculate propensities
	this->prepare();
	cerr << this->total_propensity->getDouble() << endl;
	cerr << this->propensities->at(0) << " " << this->propensities->at(1) << " " << this->propensities->at(2) << endl;

	ofstream out("gillespie_test_output.txt");
	out << "# testing output for gillespie algorithm" << endl;
	out << this->time->getDouble() << " " << this->particle_numbers->at(0) << " " << this->particle_numbers->at(1) << endl;
	while (this->time->getDouble() < 20.0) {
		this->step(0.05);
		out << this->time->getDouble() << " " << this->particle_numbers->at(0) << " " << this->particle_numbers->at(1) << endl;
	}

	delete this->reactions->at(0);
	delete this->reactions->at(1);
	delete this->reactions->at(2);
	delete this->reactions;
	delete this->total_propensity;
	delete this->propensities;
	delete this->particle_numbers;
	delete this->agent_volume;
	delete this->time;

	cerr << "#leaving the DirectSSA::testing routine" << endl;
}

void DirectSSA::printContent(ostream& out) {
	out << "# DirectSSA::printContent" << endl;
}

void DirectSSA::initialise() {
	// does not do anything ... not really needed here!
}

void DirectSSA::calcTotalPropensity() {
	this->total_propensity->setDouble(0);
	for (int i=0; i<this->reactions->size(); i++) {
		this->propensities->at(i) = reactions->at(i)->calcPropensity(particle_numbers, agent_volume->getDouble());
		this->total_propensity->addToValue(propensities->at(i));
	}
}

void DirectSSA::updatePropensity(int fired_reaction) {
	this->total_propensity->subtractFromValue(this->propensities->at(fired_reaction));
	this->propensities->at(fired_reaction) = reactions->at(fired_reaction)->calcPropensity(this->particle_numbers,
											           									   agent_volume->getDouble());
	this->total_propensity->addToValue(this->propensities->at(fired_reaction));
}
