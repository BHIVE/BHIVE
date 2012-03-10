#include "birthsimulator.hh"

using namespace Hive;

BirthSimulator::BirthSimulator(Agent *a) : Simulator() {
	this->agent = a;
	this->db	= this->agent->getDatabase();
	this->prepare();
}

BirthSimulator::~BirthSimulator() { }

void BirthSimulator::step(double t) {
	birth_flag->setBool(false);

	//when the agent exceeds the birthing threshold, then divide
	if( essence->getDouble() > essence_threshold_for_birth->getDouble() ) {

		// the blind agent divides its essence in half (which will be
		// copied into the new agent, so that the two agents split their resources)
		essence->setDouble( essence->getDouble() / 2 );

		// to be born, you should have to change directions
		DoubleData *desired_distance_to_travel    = (DoubleData*) this->agent->getDatabase()->getDataItem("distance_desired_to_travel");
		desired_distance_to_travel->setDouble(0);

		IntegerData *generationData = (IntegerData*) this->agent->getDatabase()->getDataItem("generation");
		generationData->setInteger(generationData->getInteger()+1);

		// be sure to mark this cell for division
		this->birth_flag->setBool(true);

	//	cerr<<"BirthSimulator::step(double t) === setting birth flag to true in agent: "<< agent->getAgentId() <<" now with essence: "<<this->essence->getDouble()<<endl;
	}
}

void BirthSimulator::prepare() {
	// hooking up the hooker data from the blind fart to the pimp
	essence                       = (DoubleData* ) db->getDataItem("essence");
	essence_threshold_for_birth   = (DoubleData* ) db->getDataItem("essence_threshold_for_birth");
	birth_flag                    = (BoolData* ) db->getDataItem("birth_flag");;
}

/// does not need anything.
void BirthSimulator::synchroniseWithDatabase() { }

void BirthSimulator::setAgent(Agent *ag) {
	this->agent = ag;
	this->db    = ag->getDatabase();
	this->prepare();
}

void BirthSimulator::initialise() {
	this->prepare();
}

