#include "blindagentmovementsimulator.hh"

using namespace std;
using namespace Hive;
using namespace ChemoPop;

BlindAgentMovementSimulator::BlindAgentMovementSimulator() {
	this->rho = 0;
}

void BlindAgentMovementSimulator::setAgent(Agent *ag) {
	this->agent = ag;
	this->initialise();
}

void BlindAgentMovementSimulator::step(double dt) {
	// get the
	double trav_dist = this->traveled_distance->getDouble();
	double desr_dist = this->desired_distance_to_travel->getDouble();
	double v         = this->velocity->getDouble();

	/// calculate the distance that remains to be traveled
	double remaining_distance = desr_dist -trav_dist;

	//cerr<<"remaining_distance: "<< remaining_distance <<endl;
	//cerr<<v*dt<<endl;

	/// will the move end within this time_step ?
	if (remaining_distance < v*dt) {

		/// continue traveling with a certain probability
		if (Util::RANDOM(1)*v*dt < remaining_distance) {
			/// update traveled_distance
			this->traveled_distance->setDouble(trav_dist + v*dt);
			/// set current_angle to zero
			this->current_angle->setDouble(0);
			//cerr<<"setting to zero"<<endl;
		} else {
			/// draw a new angle
			this->drawNewAngle();
			/// draw a new desired_distance
			this->drawNewDistance();
			/// set traveled_distance to having done a single step
			this->traveled_distance->setDouble(v*dt);

		//	cerr<<"drawing new angle: "<<this->current_angle->getDouble()<<endl;
		//	cerr<<"drawing new dist : "<<this->desired_distance_to_travel->getDouble()<<endl;

		}

	} else { /// need to continue in the current direction
		/// update traveled distance
		this->traveled_distance->setDouble(trav_dist + v*dt);
		/// set angle to zero
		this->current_angle->setDouble(0);
	}

}

void BlindAgentMovementSimulator::step(Agent *ag, double t) {
	this->setAgent(ag);
	this->step(t);
}

void BlindAgentMovementSimulator::initialise() {
	this->traveled_distance             = (DoubleData*) this->agent->getDatabase()->getDataItem("traveled_distance");
	this->current_angle                 = (DoubleData*) this->agent->getDatabase()->getDataItem("current_angle");
	this->desired_distance_to_travel    = (DoubleData*) this->agent->getDatabase()->getDataItem("distance_desired_to_travel");
	this->rho                           = (DoubleData*) this->agent->getDatabase()->getDataItem("rho");
	this->velocity                      = (DoubleData*) this->agent->getDatabase()->getDataItem("velocity");
	this->parameter_for_steplength_dist = (DoubleData*) this->agent->getDatabase()->getDataItem("parameter_for_steplength_dist");
}


void BlindAgentMovementSimulator::drawNewAngle() {
	/// not sure how to draw a random number according to wrapped cauchy distribution
	/// will work on this ...
	double r = this->rho->getDouble();
	double a = 1+r*r;
	double b = -2*r;

	double theta= 2*atan(pow(a*a-b*b,0.5)/(a-b)*tan(Util::RANDOM(1)*M_PI*pow(a*a-b*b,0.5)/(1-r*r)));
	this->current_angle->setDouble(abs(theta));
}
