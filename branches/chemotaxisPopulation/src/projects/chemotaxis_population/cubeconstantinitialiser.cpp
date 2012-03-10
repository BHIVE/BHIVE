#include "cubeconstantinitialiser.hh"

using namespace ChemoPop;

CubeConstantInitialiser::CubeConstantInitialiser() { }

CubeConstantInitialiser::~CubeConstantInitialiser() { }

void CubeConstantInitialiser::initialise() {
	for (int i=0; i < this->cube->getNumberOfVoxels(); i++)
		this->cube->getVoxel(i)->setConcentration(0,v);
}

void CubeConstantInitialiser::setValue(double v) {
	this->v = v;
}
