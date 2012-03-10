#include "blindAgentSimulator.hh"

using namespace ChemoPop;

BlindMonoModalAgentSimulator::BlindMonoModalAgentSimulator() { }

void BlindMonoModalAgentSimulator::setAgent(Agent *ag) {
	this->agent = ag;
}

void BlindMonoModalAgentSimulator::step(double dt) {
	/// update the time
	DoubleData* t = (DoubleData*) this->agent->getDatabase()->getDataItem("celltime");
	t->setDouble(t->getDouble()+dt);

	this->move();
}

void BlindMonoModalAgentSimulator::move() {
	IntegerData *reorient_direct = (IntegerData*) agent->getDatabase()->getDataItem("change_reorientation_direction");
	DoubleData *reorient_angle = (DoubleData*) agent->getDatabase()->getDataItem("reorientation_angle");
	DoubleData *rotational_angleA = (DoubleData*) agent->getDatabase()->getDataItem("rotational_angleA");

	/// here we would like to get an exponentially distributed run times.
	/// would be great to have something such that one can change from exponential distribution to power law behaviour!
	if (Util::RANDOM_INT(0, reorient_direct->getInteger()) == 0) {
		rotational_angleA->setDouble(Util::RANDOM(reorient_angle->getDouble()));
	}else{
		rotational_angleA->setDouble(0.0);
	}
//	cerr<<agent->getAgentId()<<": BlindMonoModalAgentSimulator::move()"<<endl;

}

BlindRandomWalkerAgentSimulator::BlindRandomWalkerAgentSimulator() { }

void BlindRandomWalkerAgentSimulator::setAgent(Agent *ag) {
	this->agent = ag;
}

void BlindRandomWalkerAgentSimulator::step(double dt) {
	DoubleData* t = (DoubleData*) this->agent->getDatabase()->getDataItem("celltime");
	t->setDouble(t->getDouble()+dt);

	this->move();
}

void BlindRandomWalkerAgentSimulator::move() {
	DoubleData *rotational_angleA = (DoubleData*) agent->getDatabase()->getDataItem("rotational_angleA");
	rotational_angleA->setDouble(Util::RANDOM(Util::PI*2.0));
}
