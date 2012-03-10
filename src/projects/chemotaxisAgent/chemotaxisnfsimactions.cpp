/*
 * chemotaxisnfsimactions.cpp
 *
 *  Created on: Oct 22, 2009
 *      Author: jentsch
 */

#include "chemotaxisnfsimactions.hh"

using namespace Chemotaxis;

UpdateLigandConcentration::UpdateLigandConcentration() {}

UpdateLigandConcentration::UpdateLigandConcentration(int id) : Action(id) {}

void UpdateLigandConcentration::fire (Data *d) {
	// do the correct cast of the data
	DoubleData *conc = (DoubleData *) d;

	DoubleData *ligand = (DoubleData *) agent->getDatabase()->getDataItem("LigandConcentration");
	ligand->setDouble(conc->getDouble());

}

