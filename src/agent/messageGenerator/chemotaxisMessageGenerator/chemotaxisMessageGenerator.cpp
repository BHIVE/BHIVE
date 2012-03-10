/**
 * @file chemotaxisMessageGenerator.cpp
 *
 *	The header file contains definitions of all message generators used
 *	in the chemotaxis system example.
 *
 * @date Oct 21, 2009   last edited: Oct 23, 2009 by Garrit and Michael
 *
 * @author Michael Sneddon
 */

#include "chemotaxisMessageGenerator.hh"
#include "../../data/primitive/primitiveData.hh"
#include "../../../exception/exception.hh"
#include "../../data/nfdata/nfdata.hh"

using namespace Hive;
using namespace Chemotaxis;


const int ChemotaxisActionIDs::UPDATE_CELL_VOXEL_POSITION_ACTION_ID = 0;
const int ChemotaxisActionIDs::UPDATE_CHEMICAL_CONCENTRATION_ACTION_ID = 1;

const int ChemotaxisActionIDs::OVERWRITE_POSITION_ACTION_ID = 0;
const int ChemotaxisActionIDs::UPDATE_SEEN_CHEMICAL_CONCENTRATION_ACTION_ID = 1;

const int ChemotaxisActionIDs::UPDATE_MOTOR_CONFORMATION_ACTION_ID = 2;

const int ChemotaxisActionIDs::UPDATE_LIGAND_CONCENTRATION_ACTION_ID = 0;


CellNotifyPosToWorldMessageGenerator::CellNotifyPosToWorldMessageGenerator(Agent *ag) : MessageGenerator(ag, true)
{
	this->db = source->getDatabase();
}

CellNotifyPosToWorldMessageGenerator::~CellNotifyPosToWorldMessageGenerator()
{

}

void CellNotifyPosToWorldMessageGenerator::placeMessage(int destID)
{
	Message *msg = new Message();
	msg->setAction(ChemotaxisActionIDs::UPDATE_CELL_VOXEL_POSITION_ACTION_ID);
	msg->setDestinationID(destID);

	DoubleVectorData *sourceDvd = (DoubleVectorData *)db->getDataItem("Position");
	DoubleVectorData *dvd = new DoubleVectorData("Position",4);
	dvd->setDouble(source->getAgentId(),0);
	dvd->setDouble(sourceDvd->getDouble(1),1);
	dvd->setDouble(sourceDvd->getDouble(2),2);
	dvd->setDouble(sourceDvd->getDouble(3),3);
	msg->setArgument(dvd);

	source->placeMessageInOutbox(msg);
}




////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


CellNotifyDeltaChemicalToWorldMessageGenerator::CellNotifyDeltaChemicalToWorldMessageGenerator(Agent *ag) : MessageGenerator(ag, true) {
	this->db = source->getDatabase();
}

CellNotifyDeltaChemicalToWorldMessageGenerator::~CellNotifyDeltaChemicalToWorldMessageGenerator()
{

}

void CellNotifyDeltaChemicalToWorldMessageGenerator::placeMessage(int destID) {
//	cerr << "1" << endl;
	Message *msg = new Message();
	msg->setAction(ChemotaxisActionIDs::UPDATE_CHEMICAL_CONCENTRATION_ACTION_ID);
	msg->setDestinationID(destID);
//	cerr<< "2" << endl;
	// provide argument vector for message
	DoubleVectorData *parameter = new DoubleVectorData("delta_chemical", 3);
	parameter->setDouble(source->getAgentId(),0);
//	cerr << "3" << endl;
	// get relevant data from cell's database
	DoubleVectorData *newChemical = (DoubleVectorData *) this->db->getDataItem("Chemical");
//	cerr << "3b" << endl;
	DoubleVectorData *oldChemical = (DoubleVectorData *) this->db->getDataItem("LastChemical");
//	cerr << "4" << endl;
	// calculate delta chemical and place it into parameter
	parameter->setDouble(newChemical->getDouble(0) - oldChemical->getDouble(0) ,1);
	parameter->setDouble(newChemical->getDouble(1) - oldChemical->getDouble(1) ,2);
//	cerr << "5" << endl;
	// place parameter into message
	msg->setArgument(parameter);

	// put message in outbox
	source->placeMessageInOutbox(msg);
}




////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

WorldNotifyPosToCellMessageGenerator::WorldNotifyPosToCellMessageGenerator(Agent *ag) : MessageGenerator(ag, false)
{
	this->db = source->getDatabase();
}

WorldNotifyPosToCellMessageGenerator::~WorldNotifyPosToCellMessageGenerator()
{

}

void WorldNotifyPosToCellMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(ChemotaxisActionIDs::OVERWRITE_POSITION_ACTION_ID);
	msg->setDestinationID(destID);

	int internal_storage_position = destID-1;
	DoubleMatrixData *sourceData = (DoubleMatrixData *)db->getDataItem("cell_coordinates");
	DoubleVectorData *parameter = new DoubleVectorData("NewPosition",3);
	parameter->setDouble(sourceData->getDouble(internal_storage_position,0),0);
	parameter->setDouble(sourceData->getDouble(internal_storage_position,1),1);
	parameter->setDouble(sourceData->getDouble(internal_storage_position,2),2);

	cerr << "i the world am sending the following position data to the cell " << endl;
	cerr << parameter->getDouble(0) << " " << parameter->getDouble(1) << " " << parameter->getDouble(2) << endl;
	msg->setArgument(parameter);

	source->placeMessageInOutbox(msg);
}





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

WorldNotifyChemicalConcentrationToCellMessageGenerator::WorldNotifyChemicalConcentrationToCellMessageGenerator(Agent *ag) :
	MessageGenerator(ag, false) {
	this->db = source->getDatabase();
}

WorldNotifyChemicalConcentrationToCellMessageGenerator::~WorldNotifyChemicalConcentrationToCellMessageGenerator() {}

void WorldNotifyChemicalConcentrationToCellMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(ChemotaxisActionIDs::UPDATE_SEEN_CHEMICAL_CONCENTRATION_ACTION_ID);
	msg->setDestinationID(destID);

	DoubleVectorData *parameter = new DoubleVectorData("NewChemicalConcentrations",3);

	// position at which agent stores relevant for the agent with destinationID destID
	int internal_storage_position = destID-1;
	// get the diffusion grid out of the database
	DiffData *sourceData = (DiffData *)db->getDataItem("diffusion_grid");
	// get the cells-voxel positions out of the database
	IntVectorData *cvp = (IntVectorData *)db->getDataItem("cells_voxel_positions");

	// get the information at which voxel the cell is stored
	int voxel_id = cvp->getInt(internal_storage_position);
	// get the voxel from the diffusion gridd
	Voxel *voxel = sourceData->getVoxel(voxel_id);
	parameter->setDouble(voxel->concentrations[0],0);
	parameter->setDouble(voxel->concentrations[1],1);
	parameter->setDouble(voxel->volume,2);
	cerr << "i am the world and iam sending " << parameter->getDouble(0) << " lig conc to cell" << endl;
	cerr << "this is the concentration from " << voxel_id << endl;
	msg->setArgument(parameter);

	source->placeMessageInOutbox(msg);
}





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

NFsimNotifyMotorConfToCellMessageGenerator::NFsimNotifyMotorConfToCellMessageGenerator(Agent *ag) : MessageGenerator(ag, true) {
	this->db = source->getDatabase();
}

NFsimNotifyMotorConfToCellMessageGenerator::~NFsimNotifyMotorConfToCellMessageGenerator() {}

void NFsimNotifyMotorConfToCellMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(ChemotaxisActionIDs::UPDATE_MOTOR_CONFORMATION_ACTION_ID);
	msg->setDestinationID(destID);


	int motorCW = 0;
	int motorCCW = 0;

	//allow error checking is something is not right by uncommenting below
	/*if(!db->existsDataItem("MotCW"))
		throw HiveException("NFsim agent needs MotorCW Data Observable!",
				"NFsimNotifyMotorConfToCellMessageGenerator::placeMessage(int destID)");

	if(!db->existsDataItem("MotCCW"))
		throw HiveException("NFsim agent needs MotorCCW Data Observable",
				"NFsimNotifyMotorConfToCellMessageGenerator::placeMessage(int destID)");*/

	Data *motCWdata = db->getDataItem("MotCW");
	if(motCWdata->getType().compare("NF_ObservableData")==0) {
		motorCW = ((NF::NF_ObservableData *)(motCWdata))->getObservableValue();
	} else {
		throw HiveException("NFsim agent needs MotorCW to be of type NF_ObservableData",
			"NFsimNotifyMotorConfToCellMessageGenerator::placeMessage(int destID)");
	}

	Data *motCCWdata = db->getDataItem("MotCCW");
	if(motCCWdata->getType().compare("NF_ObservableData")==0) {
		motorCCW = ((NF::NF_ObservableData *)motCCWdata)->getObservableValue();
	} else {
		throw HiveException("NFsim agent needs MotorCCW to be of type NF_ObservableData",
			"NFsimNotifyMotorConfToCellMessageGenerator::placeMessage(int destID)");
	}

	IntVectorData *ivd = new IntVectorData("",2);
	ivd->setInt(motorCW,0);
	ivd->setInt(motorCCW,1);
	msg->setArgument(ivd);

	source->placeMessageInOutbox(msg);
}




////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


CellNotifyLigandConcToNFsimMessageGenerator::CellNotifyLigandConcToNFsimMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db = source->getDatabase();
}

CellNotifyLigandConcToNFsimMessageGenerator::~CellNotifyLigandConcToNFsimMessageGenerator() {}

void CellNotifyLigandConcToNFsimMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(ChemotaxisActionIDs::UPDATE_LIGAND_CONCENTRATION_ACTION_ID);
	msg->setDestinationID(destID);

	// provide argument for message
	DoubleData *parameter = new DoubleData("ligand_concentration", 0);

	// get chemicals out of cells database
	//if(this->db->existsDataItem("Chemical")) cout<<"got it chief"<<endl;
	//else cout<<"nopers"<<endl;
	DoubleVectorData *chem = (DoubleVectorData*) this->db->getDataItem("Chemical");
	parameter->setDouble(chem->getDouble(0));
	cerr << "i am the cell and i am sending " << parameter->getDouble() << " as lig. to nfsim" << endl;
	// get the chemoattractant
	msg->setArgument(parameter);

	// place message
	source->placeMessageInOutbox(msg);

}
