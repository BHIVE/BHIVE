#include "bugfactory.hh"

using namespace Heatbug;

BugFactory::BugFactory() { }

BugFactory::~BugFactory() { }

Agent* BugFactory::createAgent() {
	// create agent and get his database
	Agent *bond  = new Agent();
	Database *db = bond->getDatabase();

	// --- fill database with data ---
	// time of bug
	DoubleData *bugtime = new DoubleData("bug_time",0.0);
	db->addData(bugtime);

	// dt
	// this dt is wrong but will neve be used.
	DoubleData *dt = new DoubleData("dt",1.0);
	db->addData(dt);

	// temperature which the agent prefers to have
	DoubleData *pt = new DoubleData("preferred_temperature",3.0);
	db->addData(pt);

	// output interval
	DoubleData *oi = new DoubleData("output_interval", this->output_interval);
	db->addData(oi);

	// next output time
	DoubleData *noti = new DoubleData("next_output_time", 0);
	db->addData(noti);

	// vector in which the agent stores the temperatures in its local vivicinity
	TVectorData<DoubleData> *vec = new TVectorData<DoubleData> ("local_temperature_around_me", "tvector_doubledata");
	vec->reserveSize(5);
	db->addData(vec);

	// rate with which the bug produces heat
	DoubleData *heat_rate = new DoubleData("heat_rate",1.0);
	db->addData(heat_rate);

	// direction in which the bug desires to move
	IntegerData *move_direction = new IntegerData("move_direction", 0);
	db->addData(move_direction);

	/// finally add actions, simulators, and message generators to the bug
	this->addActionsToAgentsActionSet(bond);
	this->addSimulatorToAgent(bond);
	this->addMessageGeneratorsToAgent(bond);

	// return agent
	return bond;
}

Agent* BugFactory::createAgent(Action **as) {
	throw HiveException("method not implemented","BugFactory::createAgent(Action **as)");
}

Agent* BugFactory::duplicateAgent(Agent *ag) {
	throw HiveException("method not implemented", "BugFactory::duplicateAgent(Agent *ag)");
}

void BugFactory::finalise() { }

void BugFactory::addActionsToAgentsActionSet(Agent *ag) {
	BugUpdateSeenTemperatureAction *busta = new BugUpdateSeenTemperatureAction();
	busta->setAgent(ag);
	ag->addAction(busta);
}

void BugFactory::addSimulatorToAgent(Agent *ag) {
	// add the simulator for deciding in which direction to move
	MovementDecisionSimulator *mds = new MovementDecisionSimulator();
	mds->setAgent(ag);
	ag->addSimulator(mds);
	// add time keeper simulator
	TimeKeeper *tk = new TimeKeeper(ag,"bug_time");
	ag->addSimulator(tk);
}

void BugFactory::addMessageGeneratorsToAgent(Agent *ag) {
	BugNotifyWorldAboutDesiredMovementMessageGenerator *bnwadmmg = new BugNotifyWorldAboutDesiredMovementMessageGenerator(ag);
	ag->addMessageGenerator(bnwadmmg);

	BugNotifyWorldAboutProducedHeatMessageGenerator *bnwaphmg = new BugNotifyWorldAboutProducedHeatMessageGenerator(ag);
	ag->addMessageGenerator(bnwaphmg);

	BugTellSpecialAgentAboutHappinessMessageGenerator *btsaahmg = new BugTellSpecialAgentAboutHappinessMessageGenerator(ag);
	ag->addMessageGenerator(btsaahmg);
}
