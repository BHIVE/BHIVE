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
#include "../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../core/exception/exception.hh"

using namespace Hive;
using namespace Transwell;

const int TranswellActionIDs::UPDATE_CELL_SEEN_IL2_ACTION_ID = 0;
const int TranswellActionIDs::UPDATE_CELL_SEEN_PARTICLES_ACTION_ID = 1;

const int TranswellActionIDs::UPDATE_NUCLEUS_SEEN_PSTAT5_ACTION_ID = 0;

const int TranswellActionIDs::UPDATE_WORLD_SEEN_IL2_ACTION_ID = 0;
const int TranswellActionIDs::INIT_WORLD_CELL_POSITION_ACTION_ID = 1;
const int TranswellActionIDs::INIT_WORLD_TREG_POSITION_ACTION_ID = 2;

const int TranswellActionIDs::SPECIAL_AGENT_ACCUMULATE_CELL_DATA_ACTION_ID = 0;
const int TranswellActionIDs::SPECIAL_AGENT_OUTPUT_GLOBAL_CONCENTRATIONS_ID = 1;
const int TranswellActionIDs::SPECIAL_AGENT_OUTPUT_IL2_PROFILE_ID = 2;
const int TranswellActionIDs::SPECIAL_AGENT_ACCUMULATE_TREG_DATA_ACTION_ID = 3;

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

WorldTellIL2ToSpecialAgentMessageGenerator::WorldTellIL2ToSpecialAgentMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db = source->getDatabase();
}

WorldTellIL2ToSpecialAgentMessageGenerator::~WorldTellIL2ToSpecialAgentMessageGenerator() { }

void WorldTellIL2ToSpecialAgentMessageGenerator::placeMessage(int destID) {
	IntegerData *nbo     = (IntegerData *) db->getDataItem("number_between_outputs");
	IntegerData *counter = (IntegerData *) db->getDataItem("counter");

	if (counter->getInteger() == nbo->getInteger()) {
		// setup message
		Message *msg = new Message();
		msg->setAction(TranswellActionIDs::SPECIAL_AGENT_OUTPUT_GLOBAL_CONCENTRATIONS_ID);
		msg->setDestinationID(destID);
		// data to be send
		TVectorData<double> *il2_data = new TVectorData<double>("il2_data","tvectordata_double");
		il2_data->reserveSize(3); // time, upper well particle number, lower well particle number
		// get the from the world
		TVectorData<int> *domains = (TVectorData<int> *) db->getDataItem("domain_lookup");
		Grid *g					  = (Grid*) db->getDataItem("world_agent_grid");
		DoubleData *time          = (DoubleData* ) db->getDataItem("time");
		// do the calculations
		double l = 0; double u = 0;
		if (domains != NULL) {
			for (int i = 0; i<domains->size(); i++) {
				if (domains->at(i)==1) {
			//		cerr << g->getVoxel(i)->getConcentration(0) << endl;
					u += g->getVoxel(i)->getConcentration(0);
				}
				else if (domains->at(i) == 0)
					l += g->getVoxel(i)->getConcentration(0);
			}
		} else {
			for (int i=0; i< g->getNumberOfVoxels(); i++)
				u+= g->getVoxel(i)->getConcentration(0);
		}
		il2_data->at(0) = time->getDouble();
		il2_data->at(1) = u;
		il2_data->at(2) = l;
		// set arguments
		msg->setArgument(il2_data);
		// place message
		source->placeMessageInOutbox(msg);
		counter->setInteger(1);
	} else
		counter->setInteger(counter->getInteger()+1);
}

WorldTellIL2ProfileToSpecialAgentMessageGenerator::WorldTellIL2ProfileToSpecialAgentMessageGenerator(Agent *ag): MessageGenerator(ag, false) {
	this->db = source->getDatabase();
}

WorldTellIL2ProfileToSpecialAgentMessageGenerator::~WorldTellIL2ProfileToSpecialAgentMessageGenerator() { }

void WorldTellIL2ProfileToSpecialAgentMessageGenerator::placeMessage(int destID) {
	IntegerData *counter_for_profile_output = (IntegerData *) db->getDataItem("counter_for_profile_output");
	IntegerData *nbpo = (IntegerData *) db->getDataItem("number_between_profile_output");
	if (counter_for_profile_output->getInteger() == nbpo->getInteger()) {
		Grid *g					  = (Grid*) db->getDataItem("world_agent_grid");
		DoubleData *time          = (DoubleData* ) db->getDataItem("time");
		TVectorData<int> *dl = (TVectorData<int> *) db->getDataItem("domain_lookup");
		double hmin;
		double hmax;
		double xmin;
		double xmax;
		TVectorData<double> *data = new TVectorData<double>("data", "tvectordata_double");
		// add time to data-vector
		data->addElementToEnd(time->getDouble());
		/// then the action uses this to set up a file like that one before ....
		int n = 0;
		for (int i=0; i<g->getNumberOfVoxels(); i++) {
		//	cerr << g->getCenterY(i) << endl;
//			if (((g->getCenterY(i)-0.1)<0.0001) && ((g->getCenterY(i)-0.1)>0.0)) {
			// <------------------ THIS PART HERE DEPENDS ON THE USED DISCRETISATION ------
		        // <--------- set to half of stepwidth 
			if (((g->getCenterY(i)-0.01)< 0.05) && ((g->getCenterY(i)-0.05)>0)) {
			//	cout << g->getCenterX(i) << " " << g->getCenterZ(i) << " " << g->getVoxel(i)->getConcentration(0) << endl;
				data->addElementToEnd(g->getCenterX(i));
				data->addElementToEnd(g->getCenterZ(i));
				data->addElementToEnd(g->getVoxel(i)->getConcentration(0));
			}
		}
		Message *msg = new Message();
		msg->setAction(TranswellActionIDs::SPECIAL_AGENT_OUTPUT_IL2_PROFILE_ID);
		msg->setArgument(data);
		msg->setDestinationID(destID);
		source->placeMessageInOutbox(msg);
		counter_for_profile_output->setInteger(1);
	} else
		counter_for_profile_output->setInteger(counter_for_profile_output->getInteger()+1);
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
	TVectorData<double> *pstat5 = new TVectorData<double>("pstat5", "tvectordata_double");
	pstat5->reserveSize(3);
	pstat5->at(0) = ((int)c->at(6));
	double leftover = c->at(6)-pstat5->at(0);
	if (Util::RANDOM_CLOSED() < leftover)
		pstat5->at(0) = pstat5->at(0)+1;
	pstat5->at(1) = ((DoubleData*)db->getDataItem("time"))->getDouble();
//	cerr << "sending " << pstat5->at(1) << " to cell" << endl;
	pstat5->at(2) = ((DoubleData*)db->getDataItem("propagate_until"))->getDouble();
//	IntegerData *pstat5 = new IntegerData("pstat5", 0);
	// NOTE THAT WE KNOW THAT PSTAT5 LIVES AT POSITION 6
//	pstat5->setInteger((int)c->at(6));
	msg->setArgument(pstat5);
	//	cerr << "cellNotifypstat5tonucleus sending data with type: " << pstat5->getType() << endl;
	source->placeMessageInOutbox(msg);
}

NucleusNotifyParticleNumbersToCellMessageGenerator::NucleusNotifyParticleNumbersToCellMessageGenerator(Agent *ag) : MessageGenerator(ag, true) {
	this->db = source->getDatabase();
}

NucleusNotifyParticleNumbersToCellMessageGenerator::~NucleusNotifyParticleNumbersToCellMessageGenerator() {}

void NucleusNotifyParticleNumbersToCellMessageGenerator::placeMessage(int destID) {
	// only generate a message if the flag on the database is set to true!
	if (((BoolData* )db->getDataItem("generate_message"))->getBool()) {
		Message *msg = new Message();
		msg->setAction(TranswellActionIDs::UPDATE_CELL_SEEN_PARTICLES_ACTION_ID);
		msg->setDestinationID(destID);

		TVectorData<int> *part = (TVectorData<int>* ) db->getDataItem("particle_numbers");
		TVectorData<double> *tdi = new TVectorData<double> ("numbers", "tvectordata_int");
		tdi->reserveSize(part->size()+1);
		// first element in the message is the time at which the event occurred
		tdi->at(0) = ((DoubleData *) db->getDataItem("next_reac_time"))->getDouble();
		// add information about the occupation numbers
		for (int i=0; i<part->size(); i++)
			tdi->at(i+1) = part->at(i);
		tdi->at(2) = ((DoubleData*) db->getDataItem("delta"))->getDouble() - part->at(1);
		msg->setArgument(tdi);
		// set the flag in the agent back to false
		((BoolData *)db->getDataItem("generate_message"))->setBool(false);
		// place message in outbox
		source->placeMessageInOutbox(msg);
	}
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
	dd->reserveSize(3);
	dd->at(0) = source->getAgentId();
	dd->at(1) = c->at(0);
	dd->at(2) = ((DoubleData*) db->getDataItem("time"))->getDouble();
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

TregNotifyWorldAboutPositionForInitMessageGenerator::TregNotifyWorldAboutPositionForInitMessageGenerator(Agent *ag) : MessageGenerator(ag,true) {
	this->db = source->getDatabase();
}

TregNotifyWorldAboutPositionForInitMessageGenerator::~TregNotifyWorldAboutPositionForInitMessageGenerator() {}

void TregNotifyWorldAboutPositionForInitMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(TranswellActionIDs::INIT_WORLD_TREG_POSITION_ACTION_ID);
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
	IntegerData *nbo = (IntegerData *) db->getDataItem("number_between_outputs");
	IntegerData *counter = (IntegerData *) db->getDataItem("counter");

	if (counter->getInteger() == nbo->getInteger()) {
		Message *msg = new Message();
		msg->setAction(TranswellActionIDs::SPECIAL_AGENT_ACCUMULATE_CELL_DATA_ACTION_ID);
		msg->setDestinationID(destID);

		TVectorData<double> *c = (TVectorData<double> *) db->getDataItem("concentrations");
		TVectorData<double> *dd = new TVectorData<double>("concentrations", "tvectordata_double");
		dd->reserveSize(c->size()+2);
		dd->at(0) = ((DoubleData *) db->getDataItem("time"))->getDouble();
		dd->at(1) = source->getAgentId();
		for (int i=0; i<c->size(); i++)
			dd->at(i+2) = c->at(i);
		// add argument to message
		msg->setArgument(dd);
		// place message on outbox
		source->placeMessageInOutbox(msg);
		counter->setInteger(1);
	} else {
		counter->setInteger(counter->getInteger() + 1);
	}
}

TregRevealItselfToSpecialAgentMessageGenerator::TregRevealItselfToSpecialAgentMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db = source->getDatabase();
}

TregRevealItselfToSpecialAgentMessageGenerator::~TregRevealItselfToSpecialAgentMessageGenerator() {}

void TregRevealItselfToSpecialAgentMessageGenerator::placeMessage(int destID) {
	IntegerData *nbo = (IntegerData *) db->getDataItem("number_between_outputs");
	IntegerData *counter = (IntegerData *) db->getDataItem("counter");

	if (counter->getInteger() == nbo->getInteger()) {
		Message *msg = new Message();
		msg->setAction(TranswellActionIDs::SPECIAL_AGENT_ACCUMULATE_TREG_DATA_ACTION_ID);
		msg->setDestinationID(destID);

		TVectorData<double> *c = (TVectorData<double> *) db->getDataItem("concentrations");
		TVectorData<double> *dd = new TVectorData<double>("concentrations", "tvectordata_double");
		dd->reserveSize(c->size()+1);
		dd->at(0) = source->getAgentId();
		for (int i=0; i<c->size(); i++)
			dd->at(i+1) = c->at(i);
		msg->setArgument(dd);
		// place message on outbox
		source->placeMessageInOutbox(msg);
		counter->setInteger(1);
	} else {
		counter->setInteger(counter->getInteger() + 1);
	}
}


