/*
 * SC_actions.cpp
 *
 *  Created on: Feb 15, 2010
 *      Author: msneddon
 */



#include "SC_actions.hh"
#include "../simpleCell.hh"
#include "../../../agent/action/arnie.hh"
#include "../../../agent/data/data.hh"
#include "../../../agent/data/primitive/primitiveData.hh"

using namespace std;
using namespace Hive;
using namespace SimpleCell;



UpdateLigandConcentration::UpdateLigandConcentration() {}

UpdateLigandConcentration::UpdateLigandConcentration(int id) : Action(id) {}

void UpdateLigandConcentration::fire (Data *d) {
	// perform the correct cast of the data, or unpack it if it
	// was send as a raw MPI character buffer.
	DoubleVectorData *processed;
	if(d->getType().compare("RawBufferData")==0) {
		RawBufferData *rbd = (RawBufferData *)d;
		processed = new DoubleVectorData("NewChemical",1);
		processed->unpackDataForMPI(rbd->getBuffer(), rbd->getSize());
	} else {
		processed = (DoubleVectorData *) d;
	}

	// do the correct cast of the data
	DoubleVectorData *ligand = (DoubleVectorData *) agent->getDatabase()->getDataItem("Chemical");
	ligand->setDouble(processed->getDouble(0),0);

}
