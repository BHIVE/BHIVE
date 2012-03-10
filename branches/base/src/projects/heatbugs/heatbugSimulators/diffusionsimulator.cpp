#include "diffusionsimulator.hh"

using namespace Heatbug;

DiffusionSimulator::DiffusionSimulator() {
	this->diffCoeff  = 0;
	this->dx         = 0;
	temperature_grid = NULL;
	tmp_grid         = NULL;
}

DiffusionSimulator::~DiffusionSimulator() {
	// we have to clear the tmp_grid!
}


void DiffusionSimulator::step(double t) {
	// local variables
	numCoeff = diffCoeff*dt/(dx*dx);
	double upper, right, lower, left;
	dt = t;


	// do the calculations
	for (int i=0; i<gridsize; i++) {
		for (int j=0; j<gridsize; j++) {
			// periodic boundary conditions!
			upper = temperature_grid->at(i)->at((j+1)%gridsize).getDouble();
			right = temperature_grid->at((i+1)%gridsize)->at(j).getDouble();
			lower = temperature_grid->at(i)->at((j-1+gridsize)%gridsize).getDouble();
			left  = temperature_grid->at((i-1+gridsize)%gridsize)->at(j).getDouble();
			tmp_grid->at(i)->at(j) = temperature_grid->at(i)->at(j).getDouble() +
					numCoeff*(upper+right+lower+left - 4.*temperature_grid->at(i)->at(j).getDouble());
			tmp_grid->at(i)->at(j) -= dt*dissCoeff*temperature_grid->at(i)->at(j).getDouble();
		}
	}

	// copy results into correct matrix
	for (int i=0; i<gridsize; i++)
		for (int j=0; j<gridsize; j++)
			temperature_grid->at(i)->at(j).setDouble(tmp_grid->at(i)->at(j));
}

void DiffusionSimulator::prepare() {
	Database *db = agent->getDatabase();

	// adjust pointers to correct parts of the database
	this->diffCoeff        = ((DoubleData*) db->getDataItem("diffusion_coeff"))->getDouble();
	this->dissCoeff        = ((DoubleData*) db->getDataItem("dissipation_rate"))->getDouble();
	this->dx               = 1;
	this->temperature_grid = (TVectorData<TVectorData<DoubleData>* >*) db->getDataItem("temperature_grid");

	// finally construct the tmp grid, which will be needed as a temporary memory
	gridsize = ((IntegerData*) db->getDataItem("grid_size"))->getInteger();
	tmp_grid = new TVectorData<TVectorData<double>* > ("tmp_grid","no_type_given");
	tmp_grid->reserveSize(gridsize);
	for (int i=0; i<gridsize; i++) {
		tmp_grid->at(i) = new TVectorData<double> ("no_name_given", "no_type_given");
		tmp_grid->at(i)->reserveSize(gridsize);
	}
}

void DiffusionSimulator::setAgent(Agent *ag) {
	this->agent = ag;
	this->prepare();
}

void DiffusionSimulator::setTimeStep(double t) {
	this->dt = t;
}

void DiffusionSimulator::synchroniseWithDatabase() { }

void DiffusionSimulator::initialise() { }
