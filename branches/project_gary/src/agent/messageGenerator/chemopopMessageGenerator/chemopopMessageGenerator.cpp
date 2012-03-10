
#include "../../../simulators/cellMovement/blindSearch/levyrunlengthsimulator.hh"
#include "../../../simulators/cellMovement/blindSearch/exponentialrunlengthsimulator.hh"

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
const int ChemoPopActionIDs::SPECIAL_AGENT_ACCUMULATE_BLINDAGENT_DATA_ACTION_ID = 2;
const int ChemoPopActionIDs::SPECIAL_AGENT_UPDATE_BLIND_AGENT_COUNT_ACTION_ID=3;
const int ChemoPopActionIDs::SPECIAL_AGENT_OUTPUT_GRID_ACTION_ID=4;

// hier muessen noch die ids gesetzt werden

WorldNotifyLigandToCellMessageGenerator::WorldNotifyLigandToCellMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db = source->getDatabase();

	cell_positions = (TVectorData<TVectorData<double>* >* ) db->getDataItem("cellpositions");
	isGridEnv = ((BoolData *) db->getDataItem("is_grid_environment"))->getBool();

	if(isGridEnv)
		environGrid = (Grid*) db->getDataItem("environment");
	else
		environMath = (MathFunctionData*) db->getDataItem("environment");

	t = (DoubleData*) db->getDataItem("localworldtime");
	local_index_map = (MapIntIntData*) db->getDataItem("local_index_map");
}

WorldNotifyLigandToCellMessageGenerator::~WorldNotifyLigandToCellMessageGenerator() { }

void WorldNotifyLigandToCellMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(ChemoPopActionIDs::UPDATE_CELL_SEEN_LIGAND_ACTION_ID);
	msg->setDestinationID(destID);

	/// get the cell positions from the world
	//TVectorData<TVectorData<double>* > *cell_positions = (TVectorData<TVectorData<double>* >* ) db->getDataItem("cellpositions");
	//BoolData *isGridEnv = (BoolData *) db->getDataItem("is_grid_environment");


	if(isGridEnv) {
		/// get the environment from the world
		//Grid *environGrid = (Grid*) db->getDataItem("environment");
		/// get the time from the world
		//DoubleData *t = (DoubleData*) db->getDataItem("localworldtime");
		/// get the index map
		//MapIntIntData *local_index_map = (MapIntIntData*) db->getDataItem("local_index_map");

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
		// why are we passing the movement_dt when telling the cell about the ligand ?!
		argument->at(1) = ((DoubleData*) db->getDataItem("movement_dt"))->getDouble();
		msg->setArgument(argument);

		source->placeMessageInOutbox(msg);

	} else {
		/// get the environment from the world
		//MathFunctionData *environMath = (MathFunctionData*) db->getDataItem("environment");
		/// get the time from the world
		//DoubleData *t = (DoubleData*) db->getDataItem("localworldtime");
		/// get the index map
		//MapIntIntData *local_index_map = (MapIntIntData*) db->getDataItem("local_index_map");

		/// get index of cell
		int index = destID;
		int real_index = local_index_map->returnValueForKey(index);

		vector<double> position(3);
		position[0] = cell_positions->at(real_index)->at(0);
		position[1] = cell_positions->at(real_index)->at(1);
		position[2] = cell_positions->at(real_index)->at(2);

		double new_ligand = environMath->getValue(position, t->getDouble());

		TVectorData<double> *argument = new TVectorData<double> ("argument","tvector_double");
		argument->reserveSize(2);
		argument->at(0) = new_ligand;
		// why are we passing the movement_dt when telling the cell about the ligand ?!
		argument->at(1) = ((DoubleData*) db->getDataItem("movement_dt"))->getDouble();
		msg->setArgument(argument);

		source->placeMessageInOutbox(msg);
	}


}


WorldTellSpecialAgentCellPositionsMessageGenerator::WorldTellSpecialAgentCellPositionsMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db = source->getDatabase();

	// begin output immediately, not after the first step
	nextOutputTime = 0;
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

WorldTellSpecialAgentAboutGridMessageGenerator::WorldTellSpecialAgentAboutGridMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db = source->getDatabase();
	nextOutputTime = ((DoubleData *) ag->getDatabase()->getDataItem("localworldtime"))->getDouble() + ((DoubleData *) ag->getDatabase()->getDataItem("output_interval"))->getDouble();
	g = (Grid *) db->getDataItem("environment");
}

WorldTellSpecialAgentAboutGridMessageGenerator::~WorldTellSpecialAgentAboutGridMessageGenerator() {}

void WorldTellSpecialAgentAboutGridMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(ChemoPopActionIDs::SPECIAL_AGENT_OUTPUT_GRID_ACTION_ID);
	msg->setDestinationID(destID);

	// get the relevant data from the database
	//Grid *g = (Grid *) db->getDataItem("environment");
	Coordinate c;
	double conc;
	// we will just output the concentration vector
	TVectorData<double> *argument = new TVectorData<double> ("argument", "tvector_double");

	for (int i = 0; i<g->getNumberOfVoxels(); i++) {
		conc = g->getVoxel(i)->getConcentration(0);
		g->getVoxel(i)->getCoordinate(0,c);
		argument->addElementToEnd(c.getX());
		argument->addElementToEnd(c.getY());
		argument->addElementToEnd(c.getZ());
		argument->addElementToEnd(conc);
	}
//	argument->reserveSize(g->getNumberOfVoxels());
//	for (int i=0; i<argument->size(); i++)
//		argument->at(i) = g->getVoxel(i)->getConcentration(0);

	msg->setArgument(argument);
	source->placeMessageInOutbox(msg);

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
	nextOutputTime = ((DoubleData *) ag->getDatabase()->getDataItem("celltime"))->getDouble() + ((DoubleData *) ag->getDatabase()->getDataItem("outputinterval"))->getDouble();

	t  = (DoubleData *) db->getDataItem("celltime");
	ov = (DoubleData *) db->getDataItem("outputinterval");
	cheyp = (TVectorData<double>*) db->getDataItem("CheYp");
	methLevel = (TVectorData<double>*) db->getDataItem("methLevel");
	swimmingstate = (IntegerData* ) db->getDataItem("swimming_state");
	ligand = (TVectorData<double>* ) db->getDataItem("ligands");
	accligand = (TVectorData<double>* ) db->getDataItem("accumulated_ligands");
}

CellRevealItselfToSpecialAgentMessageGenerator::~CellRevealItselfToSpecialAgentMessageGenerator() { }

void CellRevealItselfToSpecialAgentMessageGenerator::placeMessage(int destID) {
	//double t  = ((DoubleData *) db->getDataItem("celltime"))->getDouble();
	//double ov = ((DoubleData *) db->getDataItem("outputinterval"))->getDouble();

	if(t->getDouble()>=nextOutputTime) {
		nextOutputTime += ov->getDouble();

		Message *msg = new Message();
		msg->setAction(ChemoPopActionIDs::SPECIAL_AGENT_ACCUMULATE_CELL_DATA_ACTION_ID);
		msg->setDestinationID(destID);

		// get the relevant information from the cell
		// get the cheyp concentration
		//TVectorData<double> *cheyp = (TVectorData<double>*) db->getDataItem("CheYp");
		//TVectorData<double> *methLevel = (TVectorData<double>*) db->getDataItem("methLevel");

		// get the swimming state
		//IntegerData *swimmingstate = (IntegerData* ) db->getDataItem("swimming_state");

		// get the ligand concentration
		//TVectorData<double> *ligand = (TVectorData<double>* ) db->getDataItem("ligands");

		// get the accumulate ligand concentration
		//TVectorData<double> *accligand = (TVectorData<double>* ) db->getDataItem("accumulated_ligands");

		TVectorData<double> *argument = new TVectorData<double>("argument", "tvector_double");
		argument->reserveSize(2+ligand->size()+accligand->size()+cheyp->size()+methLevel->size());

		argument->at(0) = source->getAgentId();
		argument->at(1) = swimmingstate->getInteger();

		//fill up the arg array, using an improved style
		int pos = 2;

		for(int k=0; k<ligand->size(); k++,pos++)
			argument->at(pos) = ligand->at(k);

		for(int k=0; k<accligand->size(); k++,pos++)
			argument->at(pos) = accligand->at(k);

		for(int k=0; k<cheyp->size(); k++,pos++)
			argument->at(pos) = cheyp->at(k);

		for(int k=0; k<methLevel->size(); k++,pos++)
			argument->at(pos) = methLevel->at(k);

		///////// NOTE!!! If you add anything else here, be sure to update
		////////  the output agent factory file so that you reserve the correct
		///////// size in the celldatamatrix object!!

		msg->setArgument(argument);
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
