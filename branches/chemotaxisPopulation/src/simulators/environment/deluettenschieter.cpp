#include "deluettenschieter.hh"

using namespace std;
using namespace Hive;

DeLuettenSchieterSimulator::DeLuettenSchieterSimulator(Agent *a) {
//	this->setAgent(a);
	this->equilibration_time = 0;
}

/// destructor does not do anything
DeLuettenSchieterSimulator::~DeLuettenSchieterSimulator() { }

void DeLuettenSchieterSimulator::step(double t) {
	/// only do something, if the time for releasing some ... has passed.
	if (time->getDouble() > time_of_next_shitting->getDouble()) {
		// draw a new distance. exponentially distributed with distanceparameter as a parameter
		double distance = -distanceparameter->getDouble()*log(1-Util::RANDOM_CLOSED());
//		cerr << "drawing a distance of " << distance << endl;
		// calculate new position
		// generate random point on unit sphere, see numerical recipes.
		double u0, u1;
		do {
			u0 = Util::RANDOM_CLOSED() < 0.5 ? Util::RANDOM_CLOSED()*-1 : Util::RANDOM_CLOSED();
			u1 = Util::RANDOM_CLOSED() < 0.5 ? Util::RANDOM_CLOSED()*-1 : Util::RANDOM_CLOSED();
		} while(u0*u0+u1*u1 > 1);
		double x = 2.*u0*sqrt(1.-u0*u0-u1*u1);
		double y = 2.*u1*sqrt(1.-u0*u0-u1*u1);
		double z = 1-2.*(u0*u0+u1*u1);


		// get the radius correct
		x *= distance;
		y *= distance;
		z *= distance;
		//move the point
		x += this->lastposition->at(0);
		y += this->lastposition->at(1);
		z += this->lastposition->at(2);

		cerr << "# shitting event at time " << time->getDouble() << " at position " << x << "  " << y << " " << z << endl;

		// update lastposition
		this->lastposition->at(0) = x; this->lastposition->at(1) = y; this->lastposition->at(2) = z;

		// add new mathfunction to the agent's envrionment
		CarlFriedrichMathFunctionData *cf = new CarlFriedrichMathFunctionData("gauss");
		cf->setPosition(x,y,z);
		// generate initial value according to a log-normal distribution.
		// fortunately, there is a gaussian random number generator in the utility package. thanks, michael.
		double initial_value;
		if (switch_for_new_drops->getBool())
			initial_value = exp(this->initial_mean->getDouble() + this->initial_sigma->getDouble()*Util::RANDOM_GAUSSIAN());
		else
			initial_value = this->initial_mean->getDouble();
		cf->setInitialNumber(initial_value);
		cf->setDiff(this->chemo_diff->getDouble());
		this->tretmienen->addElementToEnd(cf);
		this->birthdays_of_tretmienen->addElementToEnd(this->time_of_next_shitting->getDouble());

		// calculate next event time
		this->time_of_next_shitting->setDouble(this->time->getDouble()  - timeparameter->getDouble()*log(1.-Util::RANDOM_CLOSED()));
	}
}

void DeLuettenSchieterSimulator::step(Agent *ag, double t) {
	this->setAgent(ag);
	this->step(t);
}

// nothing to do here
void DeLuettenSchieterSimulator::prepare() { }

// nothing to do here
void DeLuettenSchieterSimulator::synchroniseWithDatabase() { }

void DeLuettenSchieterSimulator::setAgent(Agent *a) {
	/// get all the data that we will need.
	this->agent = a;
	this->db    = a->getDatabase();
	this->timeparameter = (DoubleData*) db->getDataItem("environment_time_parameter");
	this->distanceparameter = (DoubleData*) db->getDataItem("environment_distance_parameter");
	this->time = (DoubleData*) db->getDataItem("localworldtime");
	this->time_of_next_shitting = (DoubleData*) db->getDataItem("environment_time_of_next_drop");
	/// initialise time_of_next_shitting in case that it is zero
	if (this->time_of_next_shitting->getDouble() == 0) {
		this->time_of_next_shitting->setDouble(this->time->getDouble() - timeparameter->getDouble()*log(1.-Util::RANDOM_CLOSED()) + this->equilibration_time);
	}
	/// get the positions
	this->tretmienen = (TVectorData<MathFunctionData*> *) db->getDataItem("environments");
	this->lastposition = (TVectorData<double> *) db->getDataItem("last_pointsource_position");
	this->lastposition->at(0) = 1000;
	this->birthdays_of_tretmienen = (TVectorData<double> *) db->getDataItem("birthtimes_of_environments");
	this->initial_mean = (DoubleData* ) db->getDataItem("mean_of_pointsource_initial_distribution");
	this->initial_sigma = (DoubleData* ) db->getDataItem("sigma_of_pointsource_initial_distribution");
	this->chemo_diff = (DoubleData*) db->getDataItem("diffusions_coeff_ligand");
	this->switch_for_new_drops = (BoolData*) db->getDataItem("switch_for_new_drops");
}

/// nothing happening here
void DeLuettenSchieterSimulator::initialise() { }


/// testing routine to see whether everything is correct.
void DeLuettenSchieterSimulator::runTest() {
	cerr << endl << endl;

	// begin by outputting the current state and what is in the database
	cerr << "# running test of DeLuettenSchieter simulator" << endl;
	cerr << "# assuming that an agent has been set to this simulator" << endl;
	cerr << "# simulation parameters are: " << endl;
	cerr << "# time parameter         " << this->timeparameter->getDouble() << endl;
	cerr << "# distance parameter     " << this->distanceparameter->getDouble() << endl;
	cerr << "# current time           " << this->time->getDouble() << endl;
	cerr << "# initial mean           " << this->initial_mean->getDouble() << endl;
	cerr << "# sigma                  " << this->initial_sigma->getDouble() << endl;
	cerr << "# next event time        " << this->time_of_next_shitting->getDouble() << endl;
	cerr << "# number of environments " << this->tretmienen->size() << endl;
	cerr << "# diff of substance      " << this->chemo_diff->getDouble() << endl;
	cerr << "# last position          " << this->lastposition->at(0) << " " << this->lastposition->at(1) << " "
			<< this->lastposition->at(2) << endl;

	// now set the time of the agent beyond the time of the next event
	cerr << "# moving forward in time and checking whether everything is updated correctly" << endl;
	this->time->setDouble(this->time_of_next_shitting->getDouble() + 0.1);
	this->step(1000);
	cerr << "# current time           " << this->time->getDouble() << endl;
	cerr << "# next event time        " << this->time_of_next_shitting->getDouble() << endl;
	cerr << "# number of environments " << this->tretmienen->size() << endl;
	cerr << "# last position          " << this->lastposition->at(0) << " " << this->lastposition->at(1) << " "
			<< this->lastposition->at(2) << endl;
	cerr << "# birth-times            " << this->birthdays_of_tretmienen->at(0) << endl;
	cerr << "# getting value at position 1,1,1 for some time ..." << endl;
	cerr << "# actually i am doing this for one hundred steps of length one" << endl;
	vector<double> hansi(3);
	hansi.at(0) = 1; hansi.at(1) = 1; hansi.at(2) = 1;
	for (int i=0; i<100; i++) {
		cout << i << " " <<  ((CarlFriedrichMathFunctionData *) tretmienen->at(0))->getValue(hansi, this->time->getDouble()+i-this->birthdays_of_tretmienen->at(0)) << endl;
	}

	cerr << endl << endl;
	//

}
