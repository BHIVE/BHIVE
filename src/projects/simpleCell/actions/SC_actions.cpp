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
	// do the correct cast of the data
	DoubleVectorData *conc = (DoubleVectorData *) d;
	DoubleVectorData *ligand = (DoubleVectorData *) agent->getDatabase()->getDataItem("Chemical");
	ligand->setDouble(conc->getDouble(0),0);

}
