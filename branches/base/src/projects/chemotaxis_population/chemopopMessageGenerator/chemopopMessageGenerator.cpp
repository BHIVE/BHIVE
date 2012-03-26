#include "../chemopopSimulators/cellMovement/blindSearch/levyrunlengthsimulator.hh"
#include "../chemopopSimulators/cellMovement/blindSearch/exponentialrunlengthsimulator.hh"

#include "chemopopMessageGenerator.hh"

using namespace Hive;
using namespace ChemoPop;

const int ChemoPopActionIDs::UPDATE_WORLD_SEEN_CELLS_SWIMMING_STATE_ACTION_ID = 0;
const int ChemoPopActionIDs::UPDATE_WORLD_SEEN_CELL_SPEED_ACTION_ID = 1;
const int ChemoPopActionIDs::UPDATE_WORLD_BACKGROUND_CONCENTRATION_ACTION_ID = 2;
const int ChemoPopActionIDs::UPDATE_WORLD_SEEN_BLINDAGENTS_ANGLES_ACTION_ID = 3;
const int ChemoPopActionIDs::UPDATE_WORLD_DESIRED_CONSUMPTION_ACTION_ID = 4;
const int ChemoPopActionIDs::UPDATE_WORLD_BLIND_AGENT_DEATH_ACTION_ID = 5;
const int ChemoPopActionIDs::UPDATE_WORLD_BLIND_AGENT_BIRTH_ACTION_ID = 6;

const int ChemoPopActionIDs::UPDATE_CELL_SEEN_LIGAND_ACTION_ID = 0;
const int ChemoPopActionIDs::UPDATE_CELL_ACCUMULATED_LIGAND_ACTION_ID = 1;

const int ChemoPopActionIDs::SPECIAL_AGENT_RECEIVE_CELL_POSITIONS_ACTION_ID = 0;
const int ChemoPopActionIDs::SPECIAL_AGENT_ACCUMULATE_CELL_DATA_ACTION_ID = 1;
const int ChemoPopActionIDs::SPECIAL_AGENT_OUTPUT_ANALYSED_CELL_POSITIONS_DATA_ACTION_ID=2;
const int ChemoPopActionIDs::SPECIAL_AGENT_OUTPUT_ANALYSED_LIGAND_DATA_ACTION_ID=3;
const int ChemoPopActionIDs::SPECIAL_AGENT_OUTPUT_ANALYSED_CELL_ACTIVITY_ACTION_ID=4;

// these ids are most likely not okay
const int ChemoPopActionIDs::SPECIAL_AGENT_ACCUMULATE_BLINDAGENT_DATA_ACTION_ID = 3;
const int ChemoPopActionIDs::SPECIAL_AGENT_UPDATE_BLIND_AGENT_COUNT_ACTION_ID=4;
const int ChemoPopActionIDs::SPECIAL_AGENT_OUTPUT_GRID_ACTION_ID=5;



WorldNotifyLigandToCellMessageGenerator::WorldNotifyLigandToCellMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db = source->getDatabase();

	cell_positions = (TVectorData<TVectorData<double>* >* ) db->getDataItem("cellpositions");
	isGridEnv = ((BoolData *) db->getDataItem("is_grid_environment"))->getBool();

	if(isGridEnv)
		environGrid = (Grid*) db->getDataItem("environment");
	else {
		environMath = (TVectorData<MathFunctionData* >*) db->getDataItem("environments");
		birthtimes  = (TVectorData<double> *) db->getDataItem("birthtimes_of_environments");
	}

	t = (DoubleData*) db->getDataItem("localworldtime");
	local_index_map = (MapIntIntData*) db->getDataItem("local_index_map");
}

WorldNotifyLigandToCellMessageGenerator::~WorldNotifyLigandToCellMessageGenerator() { }

void WorldNotifyLigandToCellMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(ChemoPopActionIDs::UPDATE_CELL_SEEN_LIGAND_ACTION_ID);
	msg->setDestinationID(destID);

	if(isGridEnv) {
		/// get index of cell
		int index = destID;
		int real_index = local_index_map->returnValueForKey(index);

		vector<double> position(3);
		position[0] = cell_positions->at(real_index)->at(0);
		position[1] = cell_positions->at(real_index)->at(1);
		position[2] = cell_positions->at(real_index)->at(2);

		double new_ligand=0;
		if(environGrid->getValue(position, t->getDouble())>0)
			new_ligand = environGrid->getValue(position, t->getDouble());

		TVectorData<double> *argument = new TVectorData<double> ("argument","tvector_double");
		argument->reserveSize(2);
		argument->at(0) = new_ligand;
		// why are we passing the dt when telling the cell about the ligand ?!
		argument->at(1) = ((DoubleData*) db->getDataItem("dt"))->getDouble();
		msg->setArgument(argument);

		source->placeMessageInOutbox(msg);

	} else { // mathematical description of the environment! there might be more than one!
		/// get index of cell
		int index = destID;
		int real_index = local_index_map->returnValueForKey(index);

		vector<double> position(3);
		position[0] = cell_positions->at(real_index)->at(0);
		position[1] = cell_positions->at(real_index)->at(1);
		position[2] = cell_positions->at(real_index)->at(2);

		/// here we need to iterate over all environments in the world.
		double new_ligand = 0;

//		cerr << "here" << endl;
//		cerr << environMath->size() << " " << birthtimes->size() << endl;
		for (int i=0; i<environMath->size(); i++)
			new_ligand += environMath->at(i)->getValue(position, t->getDouble()-birthtimes->at(i));

		TVectorData<double> *argument = new TVectorData<double> ("argument","tvector_double");
		argument->reserveSize(2);
		argument->at(0) = new_ligand;
		/// why are we telling it the dt?
		argument->at(1) = ((DoubleData*) db->getDataItem("dt"))->getDouble();
		msg->setArgument(argument);

		source->placeMessageInOutbox(msg);
	}


}


WorldTellSpecialAgentCellPositionsMessageGenerator::WorldTellSpecialAgentCellPositionsMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db = source->getDatabase();

	// begin output immediately, not after the first step
	nextOutputTime = ((DoubleData *) ag->getDatabase()->getDataItem("eqTime"))->getDouble();
	//((DoubleData *) ag->getDatabase()->getDataItem("localworldtime"))->getDouble() + ((DoubleData *) ag->getDatabase()->getDataItem("output_interval"))->getDouble();

	t  = (DoubleData *) db->getDataItem("localworldtime");
	ov = (DoubleData *) db->getDataItem("output_interval");
	positions = (TVectorData<TVectorData<double>* >*) db->getDataItem("cellpositions");
	index_agentid_map              = (MapIntIntData*) db->getDataItem("indexagentidmap");

}

WorldTellSpecialAgentCellPositionsMessageGenerator::~WorldTellSpecialAgentCellPositionsMessageGenerator() { }


void WorldTellSpecialAgentCellPositionsMessageGenerator::placeMessage(int destID) {
	//double t  = ((DoubleData *) db->getDataItem("localworldtime"))->getDouble();
	//double ov = ((DoubleData *) db->getDataItem("output_interval"))->getDouble();

	//double nextOutputTime;
	if(t->getDouble()>=nextOutputTime) {
		nextOutputTime += ov->getDouble();

		Message *msg = new Message();
		msg->setAction(ChemoPopActionIDs::SPECIAL_AGENT_RECEIVE_CELL_POSITIONS_ACTION_ID);
		msg->setDestinationID(destID);

		// get the cellposition data from the database
		//TVectorData<TVectorData<double>* > *positions = (TVectorData<TVectorData<double>* >*) db->getDataItem("cellpositions");
		//MapIntIntData *index_agentid_map              = (MapIntIntData*) db->getDataItem("indexagentidmap");

		// make a copy of the information ... message with argument is destroyed at the end of a communication step
		TVectorData<TVectorData<double>* > *argument = new TVectorData<TVectorData<double>* > ("argument", "tvector_doublematrix");
		argument->reserveSize(positions->size()+1);

		// save the time (and any other meta data in the first vector
		argument->at(0) = new TVectorData<double> ("time", "tvector_double");
		argument->at(0)->reserveSize(1);
		argument->at(0)->at(0) = t->getDouble();

		for (int i=0; i<argument->size()-1; i++) {
			argument->at(i+1) = new TVectorData<double> ("line", "tvector_double");
			argument->at(i+1)->reserveSize(4);
			argument->at(i+1)->at(0) = index_agentid_map->returnValueForKey(i);
			argument->at(i+1)->at(1) = positions->at(i)->at(0);
			argument->at(i+1)->at(2) = positions->at(i)->at(1);
			argument->at(i+1)->at(3) = positions->at(i)->at(2);
		}

		msg->setArgument(argument);

		// place message in outbox
		source->placeMessageInOutbox(msg);


		// Debugging:::
		//TVectorData<double> *speeds = (TVectorData<double>*) db->getDataItem("cellspeeds");
		//cout<<"=="<<endl;
		//for(int i=0; i<speeds->size(); i++) {
		//	cout<<"["<<i<<"]: "<<speeds->at(i)<<endl;
		//}
	}
}

WorldTellSpecialAgentAnalysedCellPositionsDataMessageGenerator::WorldTellSpecialAgentAnalysedCellPositionsDataMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db = source->getDatabase();
	nextOutputTime = ((DoubleData*) db->getDataItem("eqTime"))->getDouble();
	t  = (DoubleData *) db->getDataItem("localworldtime");
	ov = (DoubleData *) db->getDataItem("output_interval");

	positions = (TVectorData<TVectorData<double>* >* ) db->getDataItem("cellpositions");
	index_agentid_map              = (MapIntIntData*) db->getDataItem("indexagentidmap");
}

WorldTellSpecialAgentAnalysedCellPositionsDataMessageGenerator::~WorldTellSpecialAgentAnalysedCellPositionsDataMessageGenerator() { }

void WorldTellSpecialAgentAnalysedCellPositionsDataMessageGenerator::placeMessage(int destID) {
	if(t->getDouble()>=nextOutputTime) {
		// update nextoupttime
		nextOutputTime += ov->getDouble();

		// some local variables needed for analysing the data
		double avtrdist    = 0;
		double sqavtrdist  = 0;
		double stdtrdist   = 0;
		double comx        = 0;
		double com2x       = 0;
		double com3x       = 0;
		double com4x       = 0;
		double comy        = 0;
 		double comz        = 0;
		double tmp         = 0;
		double x           = 0;
		double y           = 0;
		double z           = 0;
		int num_cells      = positions->size();
		// iterate over all cell positions
		for (int i=0; i<num_cells; i++) {
			x           = positions->at(i)->at(0);
			y           = positions->at(i)->at(1);
			z           = positions->at(i)->at(2);
			// calcuate traveled distance of this cell
			tmp         = sqrt(x*x+y*y+z*z);
			// update average traveled distance
			avtrdist   += tmp;
			// for calculating the std
			sqavtrdist += tmp*tmp;
			// for calculating the center of mass
			comx  += x;
			com2x += x*x;
			com3x += x*x*x;
			com4x += x*x*x*x;
			comy  += y;
			comz  += z;
		}
		// get the mean quantities
		comx       /= num_cells;
		com2x      /= num_cells;
		com3x      /= num_cells;
		com4x      /= num_cells;
		comy       /= num_cells;
		comz       /= num_cells;
		avtrdist   /= num_cells;
		sqavtrdist /= num_cells;
		/// construct the message
		Message *msg = new Message();
		msg->setAction(ChemoPopActionIDs::SPECIAL_AGENT_OUTPUT_ANALYSED_CELL_POSITIONS_DATA_ACTION_ID);
		msg->setDestinationID(destID);
		// make a copy of the information ... message with argument is destroyed at the end of a communication step
		TVectorData<double> *argument = new TVectorData<double> ("argument", "tvector_double");
		// reserve a size 6
		argument->reserveSize(9);
		// pos 0: time
		// pos 1: average travelled distance
		// pos 2: average squared travelled distance
		// pos 3: center of mass x
		// pos 4: center of mass y
		// pos 5: center of mass z
		// pos 6: 2nd moment of x
		// pos 7: 3rd moment of x
		// pos 8: 4th moment of x
		argument->at(0) = t->getDouble();
		argument->at(1) = avtrdist;
		argument->at(2) = sqavtrdist;
		argument->at(3) = comx;
		argument->at(4) = comy;
		argument->at(5) = comz;
		argument->at(6) = com2x;
		argument->at(7) = com3x;
		argument->at(8) = com4x;
		// set argument of the message
		msg->setArgument(argument);
		// place message in outbox
		source->placeMessageInOutbox(msg);
	}
}


WorldTellSpecialAgentAboutGridMessageGenerator::WorldTellSpecialAgentAboutGridMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db = source->getDatabase();
	nextOutputTime = 0;
	t  = (DoubleData *) db->getDataItem("localworldtime");
	ov = (DoubleData *) db->getDataItem("output_interval");

	if(db->existsDataItem("environment")) {
		g = (Grid *) db->getDataItem("environment");
	} else {
		throw HiveException("when creating WorldTellSpecialAgentAboutGridMessageGenerator, no environment found.  "
				"be sure you are not using a MATH environment, in which case you "
				"should run the hive with the flag: '-blockworldoutput true' ", "WorldTellSpecialAgentAboutGridMessageGenerator::WorldTellSpecialAgentAboutGridMessageGenerator()");
	}
}

WorldTellSpecialAgentAboutGridMessageGenerator::~WorldTellSpecialAgentAboutGridMessageGenerator() {}

void WorldTellSpecialAgentAboutGridMessageGenerator::placeMessage(int destID) {
	if(t->getDouble()>=nextOutputTime) {
		nextOutputTime += ov->getDouble();

		Message *msg = new Message();
		msg->setAction(ChemoPopActionIDs::SPECIAL_AGENT_OUTPUT_GRID_ACTION_ID);
		msg->setDestinationID(destID);

		Coordinate c;
		double conc;

		TVectorData<double> *argument = new TVectorData<double> ("argument", "tvector_double");
		for (int i = 0; i<g->getNumberOfVoxels(); i++) {
			conc = g->getVoxel(i)->getConcentration(0);
			for (int j=0; j<4; j++) {
				g->getVoxel(i)->getCoordinate(j,c);
				argument->addElementToEnd(c.getX());
				argument->addElementToEnd(c.getY());
				argument->addElementToEnd(c.getZ());
				argument->addElementToEnd(conc);
			}
		}
		msg->setArgument(argument);
		source->placeMessageInOutbox(msg);
	}
}

WorldNotifyCellAboutConsumedNutrientMessageGenerator::WorldNotifyCellAboutConsumedNutrientMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db = source->getDatabase();
	dcc       = (TVectorData<double>*) db->getDataItem("desired_cell_consumption");
	local_index_map = (MapIntIntData*) db->getDataItem("local_index_map");

}

WorldNotifyCellAboutConsumedNutrientMessageGenerator::~WorldNotifyCellAboutConsumedNutrientMessageGenerator() { }

void WorldNotifyCellAboutConsumedNutrientMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(ChemoPopActionIDs::UPDATE_CELL_ACCUMULATED_LIGAND_ACTION_ID);
	msg->setDestinationID(destID);

	// get the relevant data from the database
	//TVectorData<double> *dcc       = (TVectorData<double>*) db->getDataItem("desired_cell_consumption");
	//MapIntIntData *local_index_map = (MapIntIntData*) db->getDataItem("local_index_map");

	int index      = destID;
	int real_index = local_index_map->returnValueForKey(index);

	// prepare argument
	DoubleData *argument = new DoubleData("argument", dcc->at(real_index));

	msg->setArgument(argument);
	source->placeMessageInOutbox(msg);
}

CellNotifyWorldAboutSwimmingStateMessageGenerator::CellNotifyWorldAboutSwimmingStateMessageGenerator(Agent *ag) : MessageGenerator(ag, true) {
	this->db = source->getDatabase();
	swimmingstate = (IntegerData*) db->getDataItem("swimming_state");

}

CellNotifyWorldAboutSwimmingStateMessageGenerator::~CellNotifyWorldAboutSwimmingStateMessageGenerator() { }

void CellNotifyWorldAboutSwimmingStateMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(ChemoPopActionIDs::UPDATE_WORLD_SEEN_CELLS_SWIMMING_STATE_ACTION_ID);
	msg->setDestinationID(destID);

	// get the relevant data
	//IntegerData *swimmingstate = (IntegerData*) db->getDataItem("swimming_state");

	TVectorData<int> *argument =  new TVectorData<int>("argument", "tvector_integer");
	argument->reserveSize(2);
	argument->at(0) = source->getAgentId();
	argument->at(1) = swimmingstate->getInteger();

	msg->setArgument(argument);

	source->placeMessageInOutbox(msg);
}

CellNotifyWorldAboutNewSpeedMessageGenerator::CellNotifyWorldAboutNewSpeedMessageGenerator(Agent *ag) : MessageGenerator(ag, true) {
	this->db = source->getDatabase();
}

CellNotifyWorldAboutNewSpeedMessageGenerator::~CellNotifyWorldAboutNewSpeedMessageGenerator() { }

void CellNotifyWorldAboutNewSpeedMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(ChemoPopActionIDs::UPDATE_WORLD_SEEN_CELL_SPEED_ACTION_ID);

	// get the argument and data that needs to broadcasted.
	TVectorData<double> *argument = new TVectorData<double>("argument", "tvector_double");
	argument->reserveSize(2);
	argument->at(0) = source->getAgentId();

	msg->setArgument(argument);

	source->placeMessageInOutbox(msg);

	cerr<<"CALLING BAD MESSAGE GENERATOR:: CellNotifyWorldAboutNewSpeedMessageGenerator"<<endl;
	exit(1);
}

CellRevealItselfToSpecialAgentMessageGenerator::CellRevealItselfToSpecialAgentMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db = source->getDatabase();
	nextOutputTime = ((DoubleData *) db->getDataItem("eqtime"))->getDouble();
		//	((DoubleData *) ag->getDatabase()->getDataItem("celltime"))->getDouble() + ((DoubleData *) ag->getDatabase()->getDataItem("outputinterval"))->getDouble();

	t  = (DoubleData *) db->getDataItem("celltime");
	ov = (DoubleData *) db->getDataItem("outputinterval");
	cheyp = (DoubleData *) db->getDataItem("cheyphos");
	methLevel = (DoubleData*) db->getDataItem("methyl");
	swimmingstate = (IntegerData* ) db->getDataItem("swimming_state");
	ligand = (TVectorData<double>* ) db->getDataItem("ligands");
	accligand = (TVectorData<double>* ) db->getDataItem("cell_acc_ligand");
	motorStates = (TVectorData<int>*) db->getDataItem("MotorStates");

	dLigand = (DoubleData *) db->getDataItem("dLigand");
	dCheYP = (DoubleData *) db->getDataItem("dCheYP");
	dActivity = (DoubleData *) db->getDataItem("dActivity");
	dMeth = (DoubleData *) db->getDataItem("dMeth");
}

CellRevealItselfToSpecialAgentMessageGenerator::~CellRevealItselfToSpecialAgentMessageGenerator() { }

void CellRevealItselfToSpecialAgentMessageGenerator::placeMessage(int destID) {
	//double t  = ((DoubleData *) db->getDataItem("celltime"))->getDouble();
	//double ov = ((DoubleData *) db->getDataItem("outputinterval"))->getDouble();

	if(t->getDouble()>=(nextOutputTime)) {
		nextOutputTime += ov->getDouble();

		Message *msg = new Message();
		msg->setAction(ChemoPopActionIDs::SPECIAL_AGENT_ACCUMULATE_CELL_DATA_ACTION_ID);
		msg->setDestinationID(destID);


		TVectorData<double> *argument = new TVectorData<double>("argument", "tvector_double");

		// Assumes that there is only one ligand and one accumulated ligand!
		// 0  AgentID
		// 1  Time
		// 2  Swimming State
		// 3  n_motors in CCW
		// 4  n_motors in CW
		// 5  cheYp (of motor zero if uncoordinated)
		// 6  meth  (of motor zero if uncoordinated)
		// 7  ligand
		// 8  accumulated ligand
		// 9  dLigand (OverLastDT)
		// 10 dCheYP (OverLastDT)
		// 11 dActivity (OverLastDT)
		// 12 dMeth (OverLastDT)

		argument->addElementToEnd(source->getAgentId());
		argument->addElementToEnd(t->getDouble());
		argument->addElementToEnd((double)swimmingstate->getInteger());

		int ccw_motors = 0; int cw_motors = 0;
		for(int m=0; m<motorStates->size(); m++) {
			if(motorStates->at(m)==0) ccw_motors++;
			else if(motorStates->at(m)==1) cw_motors++;
			else throw(HiveException("motor state was not zero or one!","CellRevealItselfToSpecialAgentMessageGenerator::placeMessage(int destID)"));
		}

		argument->addElementToEnd((double)ccw_motors);
		argument->addElementToEnd((double)cw_motors);

		argument->addElementToEnd((double)cheyp->getDouble());
		argument->addElementToEnd((double)methLevel->getDouble());
		argument->addElementToEnd((double)ligand->at(0));
		argument->addElementToEnd((double)accligand->at(0));



		argument->addElementToEnd((double)dLigand->getDouble());
		//cerr<<"dLig: "<<dLigand->getDouble()<<endl;
		argument->addElementToEnd((double)dCheYP->getDouble());
		argument->addElementToEnd(dActivity->getDouble());
		argument->addElementToEnd(dMeth->getDouble());



//		cerr<< "outputting ligand of: "<<ligand->at(0)<<endl;
//		for(int m=0; m<motorStates->size(); m++) {
//			if(motorStates->at(m)==0) ccw_motors++;
//			else if(motorStates->at(m)==1) cw_motors++;
//			else throw(HiveException("motor state was not zero or one!","CellRevealItselfToSpecialAgentMessageGenerator::placeMessage(int destID)"));
//		}


//		TVectorData<double> *argument = new TVectorData<double>("argument", "tvector_double");
//		argument->reserveSize(2+ligand->size()+accligand->size()+cheyp->size()+methLevel->size());
//
//		argument->at(0) = source->getAgentId();
//		argument->at(1) = swimmingstate->getInteger();
//
//		//fill up the arg array, using an improved style
//		int pos = 2;
//
//		for(int k=0; k<ligand->size(); k++,pos++)
//			argument->at(pos) = ligand->at(k);
//
//		for(int k=0; k<accligand->size(); k++,pos++)
//			argument->at(pos) = accligand->at(k);
//
//		for(int k=0; k<cheyp->size(); k++,pos++)
//			argument->at(pos) = cheyp->at(k);
//
//		for(int k=0; k<methLevel->size(); k++,pos++)
//			argument->at(pos) = methLevel->at(k);

		///////// NOTE!!! If you add anything else here, be sure to update
		////////  the output agent factory file so that you reserve the correct
		///////// size in the celldatamatrix object!!

		msg->setArgument(argument);
		source->placeMessageInOutbox(msg);
	}
}

CellTellSpecialAgentAboutSeenLigandMessageGenerator::CellTellSpecialAgentAboutSeenLigandMessageGenerator(Agent *ag) :
		MessageGenerator(ag, false) {
	this->db        = source->getDatabase();
	this->time      = (DoubleData *) this->db->getDataItem("celltime");
	this->ov        = (DoubleData *) this->db->getDataItem("outputinterval");
	this->ligand    = (TVectorData<double> *) this->db->getDataItem("ligands");
	this->accligand = (TVectorData<double> *) this->db->getDataItem("accumulated_ligands");

//	this->nextOutputTime = this->time->getDouble() + this->ov->getDouble();
	this->nextOutputTime = ((DoubleData *) db->getDataItem("eqtime"))->getDouble();
}

CellTellSpecialAgentAboutSeenLigandMessageGenerator::~CellTellSpecialAgentAboutSeenLigandMessageGenerator() { }

void CellTellSpecialAgentAboutSeenLigandMessageGenerator::placeMessage(int destID) {
	if (time->getDouble() >= nextOutputTime) {
		nextOutputTime += ov->getDouble();

		// prepare message
		Message *msg = new Message();
		msg->setAction(ChemoPopActionIDs::SPECIAL_AGENT_OUTPUT_ANALYSED_LIGAND_DATA_ACTION_ID);
		msg->setDestinationID(destID);

		// get the agent id of the sending agent
		int id = source->getAgentId();

		// set up the argument of the message
		TVectorData<double> *arg  = new TVectorData<double>("argument", "tvector_double");
		arg->reserveSize(3);
		arg->at(0) = id;
		arg->at(1) = time->getDouble();
		arg->at(2) = accligand->at(0);

		// set the message its argument and place it ins the outbox
		msg->setArgument(arg);
		source->placeMessageInOutbox(msg);
	}
}

CellTellSpecialAgentAboutActivityMessageGenerator::CellTellSpecialAgentAboutActivityMessageGenerator(Agent *ag) :
	MessageGenerator(ag, false) {
	this->db        = source->getDatabase();
	this->time      = (DoubleData *) this->db->getDataItem("celltime");
	this->ov        = (DoubleData *) this->db->getDataItem("outputinterval");

	this->activity  = (DoubleData *) this->db->getDataItem("aktivitaet");
	this->cheyp     = (DoubleData *) this->db->getDataItem("cheyphos");
	this->methyl    = (DoubleData *) this->db->getDataItem("methyl");

//	this->nextOutputTime = this->time->getDouble() + this->ov->getDouble();
//	this->nextOutputTime = ((DoubleData *) db->getDataItem("eqtime"))->getDouble();
	this->nextOutputTime = 0;
}

CellTellSpecialAgentAboutActivityMessageGenerator::~CellTellSpecialAgentAboutActivityMessageGenerator() { }

void CellTellSpecialAgentAboutActivityMessageGenerator::placeMessage(int destID) {
	if (time->getDouble() >= nextOutputTime) {
		// update next output time
		nextOutputTime += this->ov->getDouble();

		// prepare the message
		Message *msg = new Message();
		msg->setAction(ChemoPopActionIDs::SPECIAL_AGENT_OUTPUT_ANALYSED_CELL_ACTIVITY_ACTION_ID);
		msg->setDestinationID(destID);

		// add the data to argument
		TVectorData<double> *arg  = new TVectorData<double>("argument", "tvector_double");
		arg->reserveSize(4);
		arg->at(0) = time->getDouble();
		arg->at(1) = activity->getDouble();
		arg->at(2) = cheyp->getDouble();
		arg->at(3) = methyl->getDouble();

		// set the message its argument and place it ins the outbox
		msg->setArgument(arg);
		source->placeMessageInOutbox(msg);
	}
}

CellTellWorldAboutDesiredNutrientConsumptionMessageGenerator::CellTellWorldAboutDesiredNutrientConsumptionMessageGenerator(Agent *ag) :
	MessageGenerator(ag, true) {
	this->db = source->getDatabase();
	app = (TVectorData<double>* ) this->db->getDataItem("appetite");

}

CellTellWorldAboutDesiredNutrientConsumptionMessageGenerator::~CellTellWorldAboutDesiredNutrientConsumptionMessageGenerator() { }

void CellTellWorldAboutDesiredNutrientConsumptionMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(ChemoPopActionIDs::UPDATE_WORLD_DESIRED_CONSUMPTION_ACTION_ID);
	msg->setDestinationID(destID);

	// get the relevant information
	int id = source->getAgentId();

	//TVectorData<double> *app = (TVectorData<double>* ) this->db->getDataItem("appetite");

	TVectorData<double> *arg = new TVectorData<double>("argument", "tvector_double");
	arg->reserveSize(app->size()+1);

	arg->at(0) = id;
	for (int i=1; i<arg->size(); i++)
		arg->at(i) = app->at(i-1);

	msg->setArgument(arg);
	source->placeMessageInOutbox(msg);
}


BlindAgentNotifyWorldAboutAngleAndVelocityMessageGenerator::BlindAgentNotifyWorldAboutAngleAndVelocityMessageGenerator(Agent *ag) :  MessageGenerator(ag, true) {
	this->db = source->getDatabase();
	ang = (DoubleData *) this->db->getDataItem("current_angle");
	vel = (DoubleData *) this->db->getDataItem("velocity");
}

BlindAgentNotifyWorldAboutAngleAndVelocityMessageGenerator::~BlindAgentNotifyWorldAboutAngleAndVelocityMessageGenerator() {}

void BlindAgentNotifyWorldAboutAngleAndVelocityMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(ChemoPopActionIDs::UPDATE_WORLD_SEEN_BLINDAGENTS_ANGLES_ACTION_ID);
	msg->setDestinationID(destID);

	// get the relevant information
	//DoubleData *ang = (DoubleData *) this->db->getDataItem("current_angle");
	//DoubleData *vel = (DoubleData *) this->db->getDataItem("velocity");

	// set up the argument that needs to be passed around
	TVectorData<double> *argument = new TVectorData<double> ("argument", "tvector_double");
	argument->reserveSize(3);
	argument->at(0) = source->getAgentId();
	argument->at(1) = ang->getDouble();
	argument->at(2) = vel->getDouble();

	// place arguement and message
	msg->setArgument(argument);
	source->placeMessageInOutbox(msg);

  // cout<<"creating message from cell: "<<source->getAgentId()<<" new angle ="<<d->getDouble()<<" new velocity= "<<v->getDouble()<<endl;
}





BlindAgentNotifyWorldThatItIsDead::BlindAgentNotifyWorldThatItIsDead(Agent *ag) : MessageGenerator(ag,true){
	this->db = source->getDatabase();
	deathFlag = (BoolData *)db->getDataItem("death_flag");
}

BlindAgentNotifyWorldThatItIsDead::~BlindAgentNotifyWorldThatItIsDead() {}

void BlindAgentNotifyWorldThatItIsDead::placeMessage(int destID)
{
	//check if I am dead or alive
	if(deathFlag->getBool()) {

		// now die.
	//	cerr<<"BlindAgentNotifyWorldThatItIsDead::placeMessage(int destID) === sending death message from "<<source->getAgentId()<<endl;


		//since I am dead, make my farewell address
		Message *msg = new Message();
		msg->setAction(ChemoPopActionIDs::UPDATE_WORLD_BLIND_AGENT_DEATH_ACTION_ID);
		msg->setDestinationID(destID);

		//pass my agentID: that's all the info it needs (I think...)
		IntegerData *idata = new IntegerData("agent_id",source->getAgentId());
		msg->setArgument(idata);


		// put the world message in the outbox, this will still be sent in
		// the agent deconstructor as I die
		source->placeMessageInOutbox(msg);

	}
}

BlindAgentNotifyWorldThatNewAgentIsBorn::BlindAgentNotifyWorldThatNewAgentIsBorn(Agent *ag): MessageGenerator(ag,true){
	this->db = source->getDatabase();

	birthFlag = (BoolData *)db->getDataItem("birth_flag");
	deathFlag = (BoolData *)db->getDataItem("death_flag");
}

BlindAgentNotifyWorldThatNewAgentIsBorn::~BlindAgentNotifyWorldThatNewAgentIsBorn() {}

void BlindAgentNotifyWorldThatNewAgentIsBorn::placeMessage(int destID) {
	//check if I have to give birth or not
	if(birthFlag->getBool())
	{
		birthFlag->setBool(false);

		// if the death simulator determined death, then do not create the birth message
		if(deathFlag->getBool()) return;

		//get the agentfactory, and use it to create an agent
		AgentFactory *af = Registrar::getSystemRegistrar()->getAgentFactory(1);
		Agent *a = af->createAgent();

		// has the same parent
		// NOTE: the parent is the top level agent in the hierarchy of agents,
		// not the agent that gave birth!
		a->setParent(source->getParentId());

		// has the same special agents
		for(unsigned int s=0; s<source->getNumOfSpecialAgents(); s++)
			a->addSpecialAgent(source->getSpecialAgentId(s));

		// needs the same communicator, of course, of course
		a->addCommunicator(source->getCommunicator());

		// set the other properties of agent 'a' based on the source exactly...
		a->copyDatabaseInformationFromExistingAgent(this->source);


		// CLEARLY A HACK!!  CHANGE THIS AT SOME POINT TO DUPLICATE SIMULATORS
		// replace the movement simulator with the correct one
		if (((BoolData*) a->getDatabase()->getDataItem("is_levy"))->getBool()) {
			LevyRunLengthSimulator *levy = new LevyRunLengthSimulator();
			a->replaceSimulator(0,levy);
		} else {
			ExponentialRunLengthSimulator *expo = new ExponentialRunLengthSimulator();
			a->replaceSimulator(0,expo);
		}



		// ANOTHER HACK !! TIME TO NEXT OUTPUT IS OFF WHEN WE GET HERE, SO WE MUST UPDATE
		DoubleData *t  = (DoubleData *) a->getDatabase()->getDataItem("celltime");
		//DoubleData *oi = (DoubleData *) a->getDatabase()->getDataItem("outputinterval");
		//DoubleData *no = (DoubleData*) a->getDatabase()->getDataItem("nextOutputTime");

		DoubleData *dt = (DoubleData *) a->getDatabase()->getDataItem("dt");
		t->setDouble(t->getDouble()-dt->getDouble());


		//register agent a
		Message *specialMssg = new Message();
		specialMssg->setAction(ChemoPopActionIDs::SPECIAL_AGENT_UPDATE_BLIND_AGENT_COUNT_ACTION_ID);
		specialMssg->setArgument(new IntegerData("ChangeInBlindAgentNumber",1));
		Registrar::getSystemRegistrar()->registerNewAgentAndSendMessageToSpecialAgent(a,specialMssg);

		// Now, send the message to the world!!
		Message *msg = new Message();
		msg->setAction(ChemoPopActionIDs::UPDATE_WORLD_BLIND_AGENT_BIRTH_ACTION_ID);
		msg->setDestinationID(destID);

		//pass my agentID (the mother) and the new agent ID (the baby)
		TVectorData<int> *info = new TVectorData<int>("new_cell_info","tvectordata_int");
		info->addElementToEnd(this->source->getAgentId());
		info->addElementToEnd(a->getAgentId()); // get new AgentID !!!
		msg->setArgument(info);

		source->placeMessageInOutbox(msg);

	}
}



BlindAgentRevealItselfToSpecialAgentMessageGenerator::BlindAgentRevealItselfToSpecialAgentMessageGenerator(Agent *ag) : MessageGenerator(ag, true) {
	this->db = source->getDatabase();
	this->nextOutputTime = (DoubleData*) this->db->getDataItem("nextOutputTime");

	t  = (DoubleData *) db->getDataItem("celltime");
	ov = (DoubleData *) db->getDataItem("outputinterval");
	s                           = ((StringData*) this->db->getDataItem("mytypename"))->getString();
	number_of_letters                = s.size();
	dist_to_travel           = (DoubleData*) this->db->getDataItem("distance_desired_to_travel");
	essence                  = (DoubleData*) this->db->getDataItem("essence");
	km						 = (DoubleData*) this->db->getDataItem("Km");
	kcat					 = (DoubleData*) this->db->getDataItem("kcat");
	yield					 = (DoubleData*) this->db->getDataItem("yield");
	velocity                 = (DoubleData*) this->db->getDataItem("velocity");
	rho                      = (DoubleData*) this->db->getDataItem("rho");
	stepLengthDist           = (DoubleData*) this->db->getDataItem("parameter_for_steplength_dist");
	generation               = (IntegerData*) this->db->getDataItem("generation");

}

BlindAgentRevealItselfToSpecialAgentMessageGenerator::~BlindAgentRevealItselfToSpecialAgentMessageGenerator() {}

void BlindAgentRevealItselfToSpecialAgentMessageGenerator::placeMessage(int destID) {

	// get the relevant temporal information
	//double t  = ((DoubleData *) db->getDataItem("celltime"))->getDouble();
	//double ov = ((DoubleData *) db->getDataItem("outputinterval"))->getDouble();

	// only place a message if we are above the output time.
	double tol = 0.00001; // the tolerance of outputting
	if(t->getDouble()>=(nextOutputTime->getDouble()-tol)) {

		nextOutputTime->setDouble(nextOutputTime->getDouble()+ov->getDouble());
		Message *msg = new Message();
		msg->setAction(ChemoPopActionIDs::SPECIAL_AGENT_ACCUMULATE_BLINDAGENT_DATA_ACTION_ID);
		msg->setDestinationID(destID);

		/// data to be send around
		s                           = ((StringData*) this->db->getDataItem("mytypename"))->getString();
		number_of_letters           = s.size();

//		DoubleData *local_time               = (DoubleData* ) this->db->getDataItem("celltime");
//		string s                             = ((StringData*) this->db->getDataItem("mytypename"))->getString();
//		int number_of_letters                = s.size();
//		DoubleData *dist_to_travel           = (DoubleData*) this->db->getDataItem("distance_desired_to_travel");
//		DoubleData *essence                  = (DoubleData*) this->db->getDataItem("essence");
//		DoubleData *km						 = (DoubleData*) this->db->getDataItem("Km");
//		DoubleData *kcat					 = (DoubleData*) this->db->getDataItem("kcat");
//		DoubleData *yield					 = (DoubleData*) this->db->getDataItem("yield");
//		DoubleData *velocity                 = (DoubleData*) this->db->getDataItem("velocity");
//		DoubleData *rho                      = (DoubleData*) this->db->getDataItem("rho");
//		DoubleData *stepLengthDist           = (DoubleData*) this->db->getDataItem("parameter_for_steplength_dist");
//
		TVectorData<double> *argument        = new TVectorData<double> ("argument","tvector_double");

		// for sending the typename we convert it to a vector of ints
		// to decipher that on the other side we need as the first argument the number of letters in the string
		argument->addElementToEnd(source->getAgentId());
		argument->addElementToEnd(number_of_letters);
		for (int i=0; i<number_of_letters; i++)
			argument->addElementToEnd((int)s[i]);
		argument->addElementToEnd(t->getDouble());
		argument->addElementToEnd(essence->getDouble());
		argument->addElementToEnd(km->getDouble());
		argument->addElementToEnd(kcat->getDouble());
		argument->addElementToEnd(yield->getDouble());
		argument->addElementToEnd(dist_to_travel->getDouble());
		argument->addElementToEnd(velocity->getDouble());
		argument->addElementToEnd(rho->getDouble());
		argument->addElementToEnd(stepLengthDist->getDouble());
		argument->addElementToEnd(generation->getInteger());

		msg->setArgument(argument);
		source->placeMessageInOutbox(msg);
	}

}
