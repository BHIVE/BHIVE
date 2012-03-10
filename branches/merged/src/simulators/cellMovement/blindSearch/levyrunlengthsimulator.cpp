#include "levyrunlengthsimulator.hh"

using namespace std;
using namespace Hive;
using namespace ChemoPop;

LevyRunLengthSimulator::LevyRunLengthSimulator() {
	lmin = 0.002;
	lmax = 1.5;
}

LevyRunLengthSimulator::~LevyRunLengthSimulator() { }

void LevyRunLengthSimulator::drawNewDistance() {
	// draw an distance from a power law distribution
	double new_distance;
	double beta = this->parameter_for_steplength_dist->getDouble();
	new_distance = pow ((( pow(lmax, -beta+1) - pow(lmin,-beta+1) )*Util::RANDOM(1) + pow(lmin,-beta+1)),1./(-beta+1));
	this->desired_distance_to_travel->setDouble(new_distance);
}
