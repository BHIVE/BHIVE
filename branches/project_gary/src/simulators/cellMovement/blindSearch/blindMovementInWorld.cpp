/*
 * blindMovementInWorld.cpp
 *
 *  Created on: Jun 16, 2010
 *      Author: msneddon
 */

#include "blindMovementInWorld.hh"
#include "../../../agent/data/mapdata/mapintintdata.hh"

using namespace std;
using namespace Hive;
using namespace ChemoPop;

BlindMovementSimulatorInWorld::BlindMovementSimulatorInWorld() {
	this->eqTime = 0;

	// initialise the rotation Matrix to be the identity matrix
	rotMat[0][0] = 1;	rotMat[0][1] = 0;	rotMat[0][2] = 0;
	rotMat[1][0] = 0;	rotMat[1][1] = 1; 	rotMat[1][2] = 0;
	rotMat[2][0] = 0;	rotMat[2][1] = 0; 	rotMat[2][2] = 1;
}

BlindMovementSimulatorInWorld::~BlindMovementSimulatorInWorld() {}

void BlindMovementSimulatorInWorld::setWorldAgent(Agent *ag) {
	this->agent        = ag;
	this->internalTime = 0;
	this->initialise();
}

void BlindMovementSimulatorInWorld::initialise() {
	// neglecting error checking ...
	Database *db = agent->getDatabase();

	// setting the equilibration time.
	this->eqTime        = ((DoubleData*) db->getDataItem("eqTime"))->getDouble();

	// i hope i didn't screw up on the names of the data items ...
	this->numberofcells = (IntegerData*) db->getDataItem("numberofcells");
	this->movement_dt   = (DoubleData*)  db->getDataItem("movement_dt");

	this->cellspeeds               = (TVectorData<double>*) db->getDataItem("cellspeeds");
	this->cell_wants_to_move	   = (TVectorData<int>*) db->getDataItem("cell_wants_to_move");

	this->cell_positions = (TVectorData<TVectorData<double>* >* ) db->getDataItem("cellpositions");
	this->cell_dir_vecs  = (TVectorData<TVectorData<double>* >* ) db->getDataItem("celldirvecs");
	this->cell_up_vecs   = (TVectorData<TVectorData<double>* >* ) db->getDataItem("cellupvecs");

	this->nextTurnAngle  = (TVectorData <double> *) db->getDataItem("cellnextturnangle");

	this->cube_side_length = ((DoubleData *) db->getDataItem("cube_side_length"))->getDouble();

	this->isGrid = (BoolData *)db->getDataItem("is_grid_environment");
	if(isGrid->getBool())
	{
		this->g = (Grid *) db->getDataItem("environment");
		this->cell_voxel_position = (TVectorData<int> *) db->getDataItem("cell_voxel_positions");
		this->number_cells_per_voxel = (TVectorData<int> *) db->getDataItem("number_cells_per_voxel");
	}



	for (int i=0; i<this->numberofcells->getInteger(); i++) {
		this->changeDirUniformRandom(i);
	}

}

void BlindMovementSimulatorInWorld::prepare() {
	// i think there is nothing to do here.
}

void BlindMovementSimulatorInWorld::synchroniseWithDatabase() {
	// i don't think there is anything to do here.
}

void BlindMovementSimulatorInWorld::step(double t)
{
	//first keep the movement_dt up to date with the correct timestep
	this->movement_dt->setDouble(t);

	//then, make sure that we are not within the equilibration phase (in
	//which case we should not be moving at all)
	if (this->internalTime >= this->eqTime) {

		// iterate over all the cells.
		for (int i=0; i<this->numberofcells->getInteger(); i++) {
			// check whether cell wants to move
//			if (this->cell_wants_to_move->at(i)) {
				// for each cell, we first change our direction based on the next
				// turning angle for the cell
				this->changeDirBasedOnNextTurnAngle(i);
				// second, we move the cell in the correct direction according
				// to its speed and the time elapsed
				this->moveToNewPosition(i);

//			}
		}
	}



	// update the world time
	this->internalTime = this->internalTime + t;

}






void BlindMovementSimulatorInWorld::moveToNewPosition(int i) {

	double distanceTraveled = this->cellspeeds->at(i) * this->movement_dt->getDouble();


	// determine which voxel cell used to live in
	int originalVoxelIndex = -1;
	if(isGrid->getBool()) {
		originalVoxelIndex = g->insideVoxel(cell_positions->at(i)->at(0),cell_positions->at(i)->at(1),cell_positions->at(i)->at(2));
	}

	// this only works because the direction vector is always a unitvector!
	this->cell_positions->at(i)->at(0) += distanceTraveled*this->cell_dir_vecs->at(i)->at(0);
	this->cell_positions->at(i)->at(1) += distanceTraveled*this->cell_dir_vecs->at(i)->at(1);
	this->cell_positions->at(i)->at(2) += distanceTraveled*this->cell_dir_vecs->at(i)->at(2);

	if (isGrid->getBool()) {

		for(int k=0; k<3; k++) {
			if(cell_positions->at(i)->at(k)>this->cube_side_length) {
				this->cell_positions->at(i)->at(k) = this->cell_positions->at(i)->at(k) - this->cube_side_length;
			} else if (cell_positions->at(i)->at(k)<0) {
				this->cell_positions->at(i)->at(k) = this->cell_positions->at(i)->at(k) + this->cube_side_length;
			}

		}

		int nextVoxelIndex = g->insideVoxel(cell_positions->at(i)->at(0),cell_positions->at(i)->at(1),cell_positions->at(i)->at(2));
//		cerr << nextVoxelIndex << " " << cell_positions->at(i)->at(0) << " " <<
//				cell_positions->at(i)->at(1) << " " << cell_positions->at(i)->at(2) << endl;
		if(originalVoxelIndex!=nextVoxelIndex) {
			this->number_cells_per_voxel->at(originalVoxelIndex) -= 1;
			this->number_cells_per_voxel->at(nextVoxelIndex)     += 1;
			this->cell_voxel_position->at(i)=nextVoxelIndex;
		}

		// determine in which voxel the cell now lives
	}
//	cerr<<" in moveToNewPosition(int i), cell "<<i<<": "<<this->cell_positions->at(i)->at(0)<<endl;

}



void BlindMovementSimulatorInWorld::changeDirUniformRandom(int i)
{
	//Create local copies of the vectors that Util can operate on
	local_up[0] = this->cell_up_vecs->at(i)->at(0);
	local_up[1] = this->cell_up_vecs->at(i)->at(1);
	local_up[2] = this->cell_up_vecs->at(i)->at(2);

	local_dir[0] = this->cell_dir_vecs->at(i)->at(0);
	local_dir[1] = this->cell_dir_vecs->at(i)->at(1);
	local_dir[2] = this->cell_dir_vecs->at(i)->at(2);

	// generate the random rotation matrix and apply it to the direction
	// and the up position
	Util::genUniformRandRotation3d(rotMat);
	Util::applyRotation(rotMat,local_dir);
	Util::applyRotation(rotMat,local_up);


	// then update the info in the database
	this->cell_up_vecs->at(i)->at(0) = local_up[0];
	this->cell_up_vecs->at(i)->at(1) = local_up[1];
	this->cell_up_vecs->at(i)->at(2) = local_up[2];

	this->cell_dir_vecs->at(i)->at(0) = local_dir[0];
	this->cell_dir_vecs->at(i)->at(1) = local_dir[1];
	this->cell_dir_vecs->at(i)->at(2) = local_dir[2];
}


void BlindMovementSimulatorInWorld::changeDirBasedOnNextTurnAngle(int i)
{


	// First retrieve local copies of the direction and up vectors of the cells
	local_up[0] = this->cell_up_vecs->at(i)->at(0);
	local_up[1] = this->cell_up_vecs->at(i)->at(1);
	local_up[2] = this->cell_up_vecs->at(i)->at(2);

	local_dir[0] = this->cell_dir_vecs->at(i)->at(0);
	local_dir[1] = this->cell_dir_vecs->at(i)->at(1);
	local_dir[2] = this->cell_dir_vecs->at(i)->at(2);


	// Second, randomly rotate the up direction about the directional vector
	// to obtain a uniformily random new up direction.  This would come from a particular
	// distribution or may be given by the cell in the future if it wants to respond
	// to the attractant gradient.

	double randAngleInRad = Util::RANDOM_CLOSED()*2.*Util::PI;
	Util::genRot3dAboutAxis(rotMat,local_dir,randAngleInRad);
	Util::applyRotation(rotMat,local_up);


	// Third, retrieve the angle that the cell is requesting to turn by
	// and use it to orient the new direction of the cell.  For simplicity,
	// we can use the up direction as the axis of rotation, so that this
	// change in angle will always give us the correct change with respect
	// to the original direction.

	//cout<<"this->nextTurnAngle->at("<<i<<") " <<this->nextTurnAngle->at(i)<<endl;
	//cout<<" up "<<	local_up[0]<< " "<<local_up[1]<<" "<<	local_up[2] <<endl;
	//cout<<" dir"<<	local_dir[0]<< " "<<local_dir[1]<<" "<<	local_dir[2] <<endl;

	double requestedTurnAngleInRad = this->nextTurnAngle->at(i);
	Util::genRot3dAboutAxis(rotMat,local_up,requestedTurnAngleInRad);
	Util::applyRotation(rotMat,local_dir);

	//cout<<" dir"<<	local_dir[0]<< " "<<local_dir[1]<<" "<<	local_dir[2] <<endl;


	//MapIntIntData * data = (MapIntIntData *)this->agent->getDatabase()->getDataItem("indexagentidmap");
	//cout<<"agentId ="<<data->returnValueForKey(i)<<endl;


	// Finally, update the database values with the local values.
	this->cell_up_vecs->at(i)->at(0) = local_up[0];
	this->cell_up_vecs->at(i)->at(1) = local_up[1];
	this->cell_up_vecs->at(i)->at(2) = local_up[2];

	this->cell_dir_vecs->at(i)->at(0) = local_dir[0];
	this->cell_dir_vecs->at(i)->at(1) = local_dir[1];
	this->cell_dir_vecs->at(i)->at(2) = local_dir[2];

}

