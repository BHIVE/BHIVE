/**
 * @file transwellMessageGenerator.cpp
 *
 * this file contains the definitions of all message generators used in the transwell system.
 *
 * @date Jan 13, 2010
 *
 * @author garrit jentsch
 */

#include "transwellMessageGenerator.hh"
#include "../../data/primitive/primitiveData.hh"
#include "../../../exception/exception.hh"

using namespace Hive;
using namespace Transwell;

const int TranswellActionIDs::UPDATE_CELL_SEEN_IL2_ACTION_ID = 0;
const int TranswellActionIDs::UPDATE_CELL_SEEN_PARTICLES_ACTION_ID = 1;

const int TranswellActionIDs::UPDATE_NUCLEUS_SEEN_PSTAT5_ACTION_ID = 0;

const int TranswellActionIDs::UPDATE_WORLD_SEEN_IL2_ACTION_ID = 0;
const int TranswellActionIDs::INIT_WORLD_CELL_POSITION_ACTION_ID = 1;

const int TranswellActionIDs::SPECIAL_AGENT_ACCUMULATE_CELL_DATA_ACTION_ID = 0;

WorldNotifyIL2ToCellMessageGenerator::WorldNotifyIL2ToCellMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db = source->getDatabase();
}

WorldNotifyIL2ToCellMessageGenerator::~WorldNotifyIL2ToCellMessageGenerator() {}

void WorldNotifyIL2ToCellMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(TranswellActionIDs::UPDATE_CELL_SEEN_IL2_ACTION_ID);
	msg->setDestinationID(destID);

	// data to be send
	DoubleData *il2 = new DoubleData("il2", 0);
	// get the grid from the world's agentbase
	Grid *g = (Grid *) db->getDataItem("world_agent_grid");
	// get the cell positions from the world
	TVectorData<int> *pvp = (TVectorData<int> *) db->getDataItem("particles_voxel_positions");
	// get voxel position of destination cell
	int index = pvp->at(destID-1);
	// set concentration
	il2->setDouble(g->getVoxel(index)->getConcentration(0));
	msg->setArgument(il2);

	source->placeMessageInOutbox(msg);
}

CellNotifyPSTAT5ToNucleusMessageGenerator::CellNotifyPSTAT5ToNucleusMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db = source->getDatabase();
}

CellNotifyPSTAT5ToNucleusMessageGenerator::~CellNotifyPSTAT5ToNucleusMessageGenerator() {}

void CellNotifyPSTAT5ToNucleusMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(TranswellActionIDs::UPDATE_NUCLEUS_SEEN_PSTAT5_ACTION_ID);
	msg->setDestinationID(destID);
	TVectorData<double> *c = (TVectorData<double>* ) db->getDataItem("concentrations");
	IntegerData *pstat5 = new IntegerData("pstat5", 0);
	// NOTE THAT WE KNOW THAT PSTAT5 LIVES AT POSITION 6
	pstat5->setInteger((int)c->at(6));
	// interprete the fraction of pstat5
	double leftover = c->at(6)-pstat5->getInteger();
	if (Util::RANDOM_CLOSED() < leftover)
		pstat5->setInteger(pstat5->getInteger()+1);
	msg->setArgument(pstat5);
//	cerr << "cellNotifypstat5tonucleus sending data with type: " << pstat5->getType() << endl;
	source->placeMessageInOutbox(msg);
}

NucleusNotifyParticleNumbersToCellMessageGenerator::NucleusNotifyParticleNumbersToCellMessageGenerator(Agent *ag) : MessageGenerator(ag, true) {
	this->db = source->getDatabase();
}

NucleusNotifyParticleNumbersToCellMessageGenerator::~NucleusNotifyParticleNumbersToCellMessageGenerator() {}

void NucleusNotifyParticleNumbersToCellMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(TranswellActionIDs::UPDATE_CELL_SEEN_PARTICLES_ACTION_ID);
	msg->setDestinationID(destID);

	TVectorData<int> *part = (TVectorData<int>* ) db->getDataItem("particle_numbers");
	TVectorData<int> *tdi = new TVectorData<int> ("numbers", "tvectordata_int");
	tdi->reserveSize(part->size());
	for (int i=0; i<part->size(); i++)
		tdi->at(i) = part->at(i);
	msg->setArgument(tdi);

	source->placeMessageInOutbox(msg);
}

CellNotifyIL2ToWorldMessageGenerator::CellNotifyIL2ToWorldMessageGenerator(Agent *ag) : MessageGenerator(ag,true) {
	this->db = source->getDatabase();
}

CellNotifyIL2ToWorldMessageGenerator::~CellNotifyIL2ToWorldMessageGenerator() {}

void CellNotifyIL2ToWorldMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(TranswellActionIDs::UPDATE_WORLD_SEEN_IL2_ACTION_ID);
	msg->setDestinationID(destID);

	TVectorData<double> *c = (TVectorData<double> *) db->getDataItem("concentrations");
	// assuming that il2 is stored at position zero in the cell's concentrations vector
	TVectorData<double> *dd = new TVectorData<double> ("IL2", "tvectordata_double");
	dd->reserveSize(2);
	dd->at(0) = source->getAgentId();
	dd->at(1) = c->at(0);
	msg->setArgument(dd);

	source->placeMessageInOutbox(msg);
}

CellNotifyWorldAboutPositionForInitMessageGenerator::CellNotifyWorldAboutPositionForInitMessageGenerator(Agent *ag) : MessageGenerator(ag,true) {
	this->db = source->getDatabase();
}

CellNotifyWorldAboutPositionForInitMessageGenerator::~CellNotifyWorldAboutPositionForInitMessageGenerator() {}

void CellNotifyWorldAboutPositionForInitMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(TranswellActionIDs::INIT_WORLD_CELL_POSITION_ACTION_ID);
	msg->setDestinationID(destID);

	TVectorData<double> *source_pos = (TVectorData<double>* ) db->getDataItem("position");
	TVectorData<double> *vd  = new TVectorData<double>("position","tvectordata_double");
	vd->reserveSize(4);
	vd->at(0) = source->getAgentId();
	vd->at(1) = source_pos->at(0);
	vd->at(2) = source_pos->at(1);
	vd->at(3) = source_pos->at(2);
	msg->setArgument(vd);

	source->placeMessageInOutbox(msg);
}

CellRevealItselfToSpecialAgentMessageGenerator::CellRevealItselfToSpecialAgentMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db = source->getDatabase();
}

CellRevealItselfToSpecialAgentMessageGenerator::~CellRevealItselfToSpecialAgentMessageGenerator() {}

void CellRevealItselfToSpecialAgentMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(TranswellActionIDs::SPECIAL_AGENT_ACCUMULATE_CELL_DATA_ACTION_ID);
	msg->setDestinationID(destID);

	TVectorData<double> *c = (TVectorData<double> *) db->getDataItem("concentrations");
	TVectorData<double> *dd = new TVectorData<double>("concentrations", "tvectordata_double");
	dd->reserveSize(c->size()+1);
	dd->at(0) = source->getAgentId();
	for (int i=0; i<c->size(); i++)
		dd->at(i+1) = c->at(i);
	msg->setArgument(dd);

	source->placeMessageInOutbox(msg);
}
