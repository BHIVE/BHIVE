/*
 * chemotaxisworldactions.cpp
 *
 *@todo boundary conditions in updatecellvoxelpositionaction
 *
 *@author Garrit Jentsch
 *
 *@date Oct 21st, 2009 last edited 10-22-2009 by garrit
 *
 */

#include "chemotaxisworldactions.hh"

using namespace Chemotaxis;

UpdateCellVoxelPositionAction::UpdateCellVoxelPositionAction() {}

UpdateCellVoxelPositionAction::UpdateCellVoxelPositionAction(int id):Action(id){}


void UpdateCellVoxelPositionAction::fire(Data *d) {
	DoubleVectorData *pos = (DoubleVectorData* ) d;

	// get the local id of cell who has send this message
	int source = (int)pos->getDouble(0)-1; // i know that the agentIDs of the cells start at 1

	// get the diffusion grid data out of the database
	DiffData *diff_data = (DiffData*) agent->getDatabase()->getDataItem("diffusion_grid");
	//	get the cell's voxel positions out of the database
	IntVectorData *cvp = (IntVectorData*) agent->getDatabase()->getDataItem("cells_voxel_positions");
	// get the cell's positions
	DoubleMatrixData *dmd = (DoubleMatrixData*) agent->getDatabase()->getDataItem("cell_coordinates");

	// determine the new voxel in which the cell will live
	Voxel *v = diff_data->getVoxel(pos->getDouble(1), pos->getDouble(2), pos->getDouble(3));
	int newVoxelPos;

	cerr << "inside UpdateCellVoxelPosition. i am getting " << pos->getDouble(1) << " " <<
		pos->getDouble(2) << " " <<  pos->getDouble(3) << endl;
	if (v != NULL) {
		cerr << "it is inside " << v->voxel_id << endl;
	}
	if (v != NULL) {
		newVoxelPos = v->voxel_id;
		// update Cells voxel position in the world agent
		cvp->setInt(newVoxelPos, source);
		// update cells position in the world agent
		dmd->setDouble(pos->getDouble(1), source, 0);
		dmd->setDouble(pos->getDouble(2), source, 1);
		dmd->setDouble(pos->getDouble(3), source, 2);
	}
}

UpdateChemicalConcentrationAction::UpdateChemicalConcentrationAction() {}

UpdateChemicalConcentrationAction::UpdateChemicalConcentrationAction(int id):Action(id) {}

void UpdateChemicalConcentrationAction::fire(Data *d) {
	// cast the argument into the correct form
	DoubleVectorData *conc = (DoubleVectorData* ) d;
	// get the local id of cell who has send this message
	int source = (int)conc->getDouble(0)-1; // i know that the agentIDs of the cells start at 1
	// get the cells' voxel positions out of the database
	IntVectorData *cvp = (IntVectorData*) agent->getDatabase()->getDataItem("cells_voxel_positions");
	// extract in which voxel the cell who has sent this message is living
	int voxelID = cvp->getInt(source);
	// get the diff_data grid out of the database
	DiffData *diff_data = (DiffData*) agent->getDatabase()->getDataItem("diffusion_grid");
	// update the concentrations in the diff_data
	for (int i=0; i<conc->getNumElements()-1; i++)
		diff_data->getVoxel(voxelID)->concentrations[i] += conc->getDouble(i+1);
}
