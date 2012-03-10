/*
 * nf.cpp
 *
 *  Created on: Oct 20, 2009
 *      Author: msneddon
 */



#include "nf.hh"
#include "../../exception/exception.hh"
#include "../../agent/data/primitive/primitiveData.hh"


using namespace Hive;
using namespace NF;


NFsim::NFsim(Agent *a, System *s) : Simulator()
{
	// Step 1: Make sure database is set up properly to handle NFsim
	Database *db = a->getDatabase();
	if(!db->existsDataType("NF_MoleculeTypeData")) {
		throw HiveException("Agent database does not contain any MoleculeType Data",
				"NFsim::NFsim(Agent *a [with id: "+Util::toString(a->getAgentId())+"])");
	} else {
		cout<<"Found MoleculeTypeData in the db!!"<<endl;
	}

	////////CHEMOTAXIS ADDITION/////////////
	if(!db->existsDataItem("LigandConcentration")) {
		throw HiveException("Right now, the NFsim database requires a LigandConcentration DoubleData",
				"NFsim::NFsim(Agent *a [with id: "+Util::toString(a->getAgentId())+"])");
	} else {
		cout<<"Found MoleculeTypeData in the db!!"<<endl;
	}
	/////////////////////////////////////////

	// save off the agent
	this->agent=a;
	this->system=s;
}


NFsim::~NFsim()
{
	delete system;
}


void NFsim::step(double t)
{
	this->system->stepTo(system->getCurrentTime()+t);
	//this->system->sim(t,10);
}


void NFsim::prepare()
{

}



void NFsim::synchroniseWithDatabase()
{
	DoubleData *lc = (DoubleData *)(agent->getDatabase()->getDataItem("LigandConcentration"));

	////////CHEMOTAXIS ADDITION/////////////
	cout<<"updating lig conc to: "<<lc->getDouble()<<endl;
	system->setParameter("LigConc",lc->getDouble());
	system->updateSystemWithNewParameters();
	/////////////////////////////////////////

	this->system->recompute_A_tot();
}


void NFsim::initialise()
{

}
