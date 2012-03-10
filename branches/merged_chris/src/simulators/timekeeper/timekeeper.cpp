#include "timekeeper.hh"

using namespace Hive;

TimeKeeper::TimeKeeper(Agent *a, string clockName) : Simulator() {
	this->agent = a;
	this->db	= this->agent->getDatabase();
	this->clockName = clockName;
	this->prepare();
}

TimeKeeper::~TimeKeeper() { }

void TimeKeeper::step(double dt) {
	cell_time->addToValue(dt);
	this->dt->setDouble(dt);
}

void TimeKeeper::prepare()
{
	this->cell_time  = (DoubleData*) db->getDataItem(clockName);
	this->dt         = (DoubleData*) db->getDataItem("dt");
}

/// does not need anything.
void TimeKeeper::synchroniseWithDatabase() { }

void TimeKeeper::setAgent(Agent *ag) {
	this->agent = ag;
	this->db    = ag->getDatabase();
	this->prepare();
}

void TimeKeeper::initialise() {
	this->prepare();
}

