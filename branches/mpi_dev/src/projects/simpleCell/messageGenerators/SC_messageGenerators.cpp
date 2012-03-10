/*
 * SC_messageGenerators.cpp
 *
 *  Created on: Feb 15, 2010
 *      Author: msneddon
 */



#include "SC_messageGenerators.hh"
#include "../simpleCell.hh"
#include "../../../agent/messageGenerator/messagegenerator.hh"
#include "../../../agent/action/arnie.hh"
#include "../../../agent/data/data.hh"
#include "../../../agent/data/primitive/primitiveData.hh"

using namespace std;
using namespace Hive;
using namespace SimpleCell;



WorldNotifyChemicalConcentrationToCellMessageGenerator::WorldNotifyChemicalConcentrationToCellMessageGenerator(Agent *ag) :
	MessageGenerator(ag, false) {
	this->db = source->getDatabase();
}

WorldNotifyChemicalConcentrationToCellMessageGenerator::~WorldNotifyChemicalConcentrationToCellMessageGenerator() {}

void WorldNotifyChemicalConcentrationToCellMessageGenerator::placeMessage(int destID,int node) {

	Message *msg = new Message();
	msg->setAction(0);
	msg->setDestinationID(destID,node);
//	cerr << "placing message to " << destID << " on " << node << endl;
	DoubleVectorData *ligand = new DoubleVectorData("NewChemicals",1);
	ligand->setDouble(0.024,0);
	msg->setArgument(ligand);
	source->placeMessageInOutbox(msg);


//
//	// position at which agent stores relevant for the agent with destinationID destID
//	int internal_storage_position = destID-1;
//	// get the diffusion grid out of the database
//	DiffData *sourceData = (DiffData *)db->getDataItem("diffusion_grid");
//	// get the cells-voxel positions out of the database
//	IntVectorData *cvp = (IntVectorData *)db->getDataItem("cells_voxel_positions");
//
//	// get the information at which voxel the cell is stored
//	int voxel_id = cvp->getInt(internal_storage_position);
//	// get the voxel from the diffusion gridd
//	Voxel *voxel = sourceData->getVoxel(voxel_id);
//	parameter->setDouble(voxel->concentrations[0],0);
//	parameter->setDouble(voxel->concentrations[1],1);
//	parameter->setDouble(voxel->volume,2);
//	cerr << "i am the world and iam sending " << parameter->getDouble(0) << " lig conc to cell" << endl;
//	cerr << "this is the concentration from " << voxel_id << endl;
//	msg->setArgument(parameter);
//
//	source->placeMessageInOutbox(msg);
}
