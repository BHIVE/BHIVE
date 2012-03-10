/*
 * PDESimulator.cpp
 *
 *  Created on: Oct 19, 2009
 *      Author: Christopher Henry and Garrit Jentsch
 */


#include "FBASimulator.hh"

using namespace Chemotaxis;

/// Constructor
FBASimulator::FBASimulator()
{
	//For now, do nothing in the constructor
	agent = NULL;
	db = NULL;
}

/// destructor
FBASimulator::~FBASimulator()
{
}

void FBASimulator::setAgent(Agent* a)
{
	agent = a;
	db = (FBAData* )a->getDatabase()->getDataItem("fba_network");
	this->initialise();
}

void FBASimulator::step(double simdt)
{
	DoubleVectorData* DataItem = (DoubleVectorData*)agent->getDatabase()->getDataItem("Chemical");
	DoubleVectorData* LastDataItem = (DoubleVectorData*)agent->getDatabase()->getDataItem("LastChemical");
	for (int i=0; i < 2; i++) {
		LastDataItem->setDouble(DataItem->getDouble(i),i);
	}
	db->getCompound("Nutrient")->concentration = DataItem->getDouble(0);
	db->getCompound("Waste")->concentration = DataItem->getDouble(1);
	double volume = DataItem->getDouble(2);
	
	if (db == NULL) {
		cerr << "No database found in FBASimulator... bailing out!" << endl;
	}

	//determining time step
	double time_step = dt;
	int numberSteps = int(simdt/dt);
	if (dt > simdt) {
		time_step = simdt;
		numberSteps = 1;
	}

	double rate = 0;
	for (int m=0; m < numberSteps; m++) {
		//Scanning through reactions and finding the rates of reactions with kinetic constants
		for (int i=0; i < db->getNumberOfReactions(); i++) {
			Reaction* current = db->getReaction(i);
			if (current->e != -1) {
				rate = calculateRate(current);
				db->getCompound("Nutrient")->concentration = (volume*db->getCompound("Nutrient")->concentration-rate*time_step)/volume;
				db->getCompound("Waste")->concentration = (volume*db->getCompound("Waste")->concentration+rate*time_step)/volume;
				db->getCompound("Biomass")->concentration = (volume*db->getCompound("Biomass")->concentration+rate*time_step)/volume;
				i = 1000;
			}
		}
	}

	DataItem->setDouble(db->getCompound("Nutrient")->concentration,0);
	DataItem->setDouble(db->getCompound("Waste")->concentration,1);
}

void FBASimulator::prepare()
{
	//Maybe we set the "dt" element of the simulator here?
	cout<<"FBA simulator of agent: "<<this->agent->getAgentId()<<" is preparing."<<endl;
}

void FBASimulator::synchroniseWithDatabase()
{
	//This exists to update propensities in stochastic simulations... I don't expect to use this.
	cout<<"FBA simulator of agent: "<<this->agent->getAgentId()<<" is synching with database."<<endl;
}

void FBASimulator::initialise()
{
	//Set dt... perhaps this is the same functionality as prepare?
	dt = 0.001;
}

double FBASimulator::calculateRate(Reaction* inReaction)
{
	return inReaction->k1*inReaction->e*db->getCompound("Nutrient")->concentration/(inReaction->k2+db->getCompound("Nutrient")->concentration);
}