#include "exponentialrunlengthsimulator.hh"

using namespace std;
using namespace Hive;
using namespace ChemoPop;

ExponentialRunLengthSimulator::ExponentialRunLengthSimulator() {}

ExponentialRunLengthSimulator::~ExponentialRunLengthSimulator() { }

void ExponentialRunLengthSimulator::drawNewDistance() {
	// draw an exponentially distributed length
	this->desired_distance_to_travel->setDouble(-1.*this->parameter_for_steplength_dist->getDouble()*log(Util::RANDOM(1)));
	//cerr<<"setting new exponential distance: "<<desired_distance_to_travel->getDouble()<<endl;
}
