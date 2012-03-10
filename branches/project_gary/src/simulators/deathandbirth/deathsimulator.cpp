#include "deathsimulator.hh"

using namespace Hive;

DeathSimulator::DeathSimulator(Agent *a) : Simulator() {
	this->agent = a;
	this->db	= this->agent->getDatabase();
	this->prepare();
}

DeathSimulator::~DeathSimulator() { }

void DeathSimulator::step(double dt) {
	death_flag->setBool(false);

	if( essence->getDouble() < essence_threshold_for_death->getDouble() )
	{
		birth_flag->setBool(false);
		death_flag->setBool(true);
		//cerr<<"DeathSimulator::step(double t) === setting death flag to true in agent: "<< agent->getAgentId() <<endl;
		return;
	}
	// spontaneous death rate imposed by the environment.
	if (Util::RANDOM(1) < background_death_rate_per_unit_time->getDouble() * dt)
	{
		death_flag->setBool(true);
		//cerr<<"DeathSimulator::step(double t) === setting death flag to true in agent: "<< agent->getAgentId() <<endl;
	}
}

void DeathSimulator::prepare()
{
	essence                                = (DoubleData* ) db->getDataItem("essence");
	essence_threshold_for_death            = (DoubleData* ) db->getDataItem("essence_threshold_for_death");
	background_death_rate_per_unit_time    = (DoubleData* ) db->getDataItem("background_death_rate_per_unit_time");
	death_flag                             = (BoolData*)    db->getDataItem("death_flag");
	birth_flag                             = (BoolData*) db->getDataItem("birth_flag");
}

/// does not need anything.
void DeathSimulator::synchroniseWithDatabase() { }

void DeathSimulator::setAgent(Agent *ag) {
	this->agent = ag;
	this->db    = ag->getDatabase();
	this->prepare();
}

void DeathSimulator::initialise() {
	this->prepare();
}

