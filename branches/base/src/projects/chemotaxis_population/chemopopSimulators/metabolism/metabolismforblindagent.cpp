#include "metabolismforblindagent.hh"

using namespace Hive;

MetabolismForBlindAgentSimulator::MetabolismForBlindAgentSimulator(Agent *a) {
	this->agent = a;
	this->db    = a->getDatabase();
	this->prepare();
}

/// destructor
MetabolismForBlindAgentSimulator::~MetabolismForBlindAgentSimulator() { }

void MetabolismForBlindAgentSimulator::step(double dt)
{
	// updating of cell time is done in the movement simulator
	// calculate how much of your essence you spent on life and movement, and subtract it

	double dE = dt * (  -essence->getDouble()*r1->getDouble()  // house keeping cost, in one unit of essence
				        -essence_cost_for_movement->getDouble()*(pow(velocity->getDouble(),2)) );  // movement cost

	this->essence->setDouble(essence->getDouble()+dE);
}

void MetabolismForBlindAgentSimulator::prepare()
{
	essence                    = (DoubleData*) db->getDataItem("essence");
	essence_cost_for_movement  = (DoubleData*) db->getDataItem("essence_cost_for_movement");
	velocity                   = (DoubleData*) db->getDataItem("velocity");
	r1                         = (DoubleData*) db->getDataItem("r1");
}

void MetabolismForBlindAgentSimulator::synchroniseWithDatabase() { }

void MetabolismForBlindAgentSimulator::setAgent(Agent *ag) {
	this->agent = ag;
	this->db    = ag->getDatabase();
	this->prepare();
}


void MetabolismForBlindAgentSimulator::initialise() {
	this->prepare();
}

