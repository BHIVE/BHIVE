/*
 * PDESimulator.cpp
 *
 *  Created on: Oct 19, 2009
 *      Author: Christopher Henry and Garrit Jentsch
 */


#include "PDESimulator.hh"

using namespace Chemotaxis;

/// Constructor
PDESimulator::PDESimulator()
{
	//For now, do nothing in the constructor
	agent = NULL;
	db = NULL;
}

/// destructor
PDESimulator::~PDESimulator()
{
}

void PDESimulator::setAgent(Agent* a)
{
	agent = a;
	db = (DiffData* )a->getDatabase()->getDataItem("diffusion_grid");
	this->initialise();
}

void PDESimulator::step(double simdt)
{
	if (db == NULL) {
		//print error
	}
	//cout << "PDE start: " <<  db->getVoxel(34232)->concentrations[0] << " " << db->getVoxel(4950)->concentrations[0] <<endl;
	int numberSteps = int(simdt/dt);
//	cerr << numberSteps << " " << db->getNumberOfVoxels() <<  endl;
//	cout << "beginning " <<  db->getVoxel(100)->concentrations[0] << endl;
	for (int m=0; m < numberSteps; m++) {
//		cerr << m << endl;
		for (int i=0; i < db->getNumberOfVoxels(); i++) {
			Voxel* currentVoxel = db->getVoxel(i);
			stepVoxel(currentVoxel, i, dt);
		}
//		cout << "after step" << endl;
//		cout << db->getVoxel(100)->concentrations[0] << endl;
//		cout << db->getVoxel(100)->newConcentrations[0] << endl;
//		cout << db->getVoxel(100)->neighbors.size() << endl;
		for (int i=0; i < db->getNumberOfVoxels(); i++) {
			Voxel* currentVoxel = db->getVoxel(i);
			swapVoxel(currentVoxel);
		}
//		cout << "after swap" << endl;
//		cout << db->getVoxel(100)->newConcentrations[0] << endl;
//		cout << db->getVoxel(4950)->concentrations[0] << endl;
	}
	//cout << "PDE end " << db->getVoxel(34232)->concentrations[0] << " " << db->getVoxel(4950)->concentrations[0]  << endl;

}

void PDESimulator::prepare()
{
//	cerr<< "PDE simulator of agent: "<< this->agent->getAgentId()<< " is preparing."<<endl;
}

void PDESimulator::synchroniseWithDatabase()
{
//	cerr << "dummy simulator of agent: "<< this->agent->getAgentId() << " is synching with database."<<endl;
}

void PDESimulator::initialise()
{
	//Set dt
	dt = 0.0001;
}

void PDESimulator::stepVoxel(Voxel* currentVoxel, int index, double dt)
{
	double di = 0;
	double flux =0;
	for (unsigned int i=0; i < currentVoxel->concentrations.size(); i++) {
		di = db->getDiffusionCoef(i);
//		cerr << "neigh_size: " << currentVoxel->neighbors.size() << endl;
		for (unsigned int j=0; j < currentVoxel->neighbors.size(); j++) {
			flux = currentVoxel->concentrations[i]*currentVoxel->baseRates[j]*di*dt;
		//	cerr << flux << endl;
//			if (index ==100)
//				cerr << "flux " << flux << " " << currentVoxel->baseRates[j] << endl;
			currentVoxel->newConcentrations[i] -= flux;
			db->getVoxel(currentVoxel->neighbors[j])->newConcentrations[i] += flux;
		}
	}
}

void PDESimulator::swapVoxel(Voxel* currentVoxel)
{
//	currentVoxel->concentrations = currentVoxel->newConcentrations;
	for (unsigned int i=0; i<currentVoxel->newConcentrations.size(); i++) {
		currentVoxel->concentrations[i] += currentVoxel->newConcentrations[i];
		currentVoxel->newConcentrations[i] = 0; // currentVoxel->concentrations[i];
	}
}
