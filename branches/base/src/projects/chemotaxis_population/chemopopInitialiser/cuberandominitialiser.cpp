#include "cuberandominitialiser.hh"

using namespace ChemoPop;

CubeRandomInitialiser::CubeRandomInitialiser() { }

CubeRandomInitialiser::~CubeRandomInitialiser() { }

void CubeRandomInitialiser::initialise() {
	int number_voxels = cube->getNumberOfVoxels();
	int pos           = 0;
	double amount     = 0;
	for (int i=0; i<this->number_of_points; i++) {
		// making sure that one always draws an empty field, not very efficient
		// but it will work
		do {
			pos    = (int)(Util::RANDOM_CLOSED()*number_voxels);
			amount = minimum + (maximum-minimum)*Util::RANDOM_CLOSED();
		} while (cube->getVoxel(pos)->getConcentration(0) != 0);
		cube->getVoxel(pos)->setConcentration(0,amount);
	}
}

void CubeRandomInitialiser::setNumberOfPoints(int num) {
	this->number_of_points = num;
	if (this->number_of_points > cube->getNumberOfVoxels())
		this->number_of_points = cube->getNumberOfVoxels();
}

void CubeRandomInitialiser::setNutrientRange(double min, double max) {
	this->minimum = min;
	this->maximum = max;
}
