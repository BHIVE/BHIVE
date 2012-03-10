#include "chemopopAction.hh"

using namespace Hive;
using namespace ChemoPop;

UpdateWorldSeenCellsSwimmingStateAction::UpdateWorldSeenCellsSwimmingStateAction() :
	Action(ChemoPopActionIDs::UPDATE_WORLD_SEEN_CELLS_SWIMMING_STATE_ACTION_ID){}

UpdateWorldSeenCellsSwimmingStateAction::~UpdateWorldSeenCellsSwimmingStateAction() { }

void UpdateWorldSeenCellsSwimmingStateAction::prepare()
{
	if(this->agent->getDatabase()->getDataItem("swimmingstates")) {
	local_index_map = (MapIntIntData* ) this->agent->getDatabase()->getDataItem("local_index_map");
	swimstates   = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("swimmingstates");
	oldstates    = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("lastswimmingstates");
	} else {
		swimstates = 0;
		oldstates = 0;
		local_index_map = 0;
	}
}

void UpdateWorldSeenCellsSwimmingStateAction::fire(Data *d) {
	if (this->agent == NULL)
		throw HiveException("Action was not assigned to an agent", "UpdateWorldSeenCellsSwimmmingStateAction(Data *d)");
	if (d->getType().compare("tvector_integer")!=0)
		throw HiveException("wrong data passed to action", "UpdateWorldSeenCellsSwimmingStateAction::fire(Data *d)");

	TVectorData<int> *argument = (TVectorData<int> *) d;
	int cell_index             = argument->at(0);
	int newstate               = argument->at(1);

	int real_index                 = local_index_map->returnValueForKey(cell_index);
	oldstates->at(real_index)      = swimstates->at(real_index);
	swimstates->at(real_index)     = newstate;
}



UpdateWorldSeenCellSpeedAction::UpdateWorldSeenCellSpeedAction() :
	Action(ChemoPopActionIDs::UPDATE_WORLD_SEEN_CELL_SPEED_ACTION_ID) {}

UpdateWorldSeenCellSpeedAction::~UpdateWorldSeenCellSpeedAction() { }

void UpdateWorldSeenCellSpeedAction::prepare() {

	local_index_map    = (MapIntIntData*) this->agent->getDatabase()->getDataItem("local_index_map");
	cellspeeds   = (TVectorData<double>* ) this->agent->getDatabase()->getDataItem("cellspeeds");
}


void UpdateWorldSeenCellSpeedAction::fire(Data *d) {
	if (this->agent == NULL)
		throw HiveException("Action was not assigned to an agent", "UpdateWorldSeenCellSpeedAction(Data *d)");
	if (d->getType().compare("tvector_double")!=0)
		throw HiveException("wrong data passed to action", "UpdateWorldSeenCellSpeedAction::fire(Data *d)");

	TVectorData<double> *argument = (TVectorData<double>* ) d;
	int cell_position             = (int)argument->at(0);
	double newspeed				  = argument->at(1);

	int real_index                    = local_index_map->returnValueForKey(cell_position);
	cellspeeds->at(real_index)        = newspeed;

}


UpdateWorldSeenBlindAgentsAnglesAction::UpdateWorldSeenBlindAgentsAnglesAction() :
	Action(ChemoPopActionIDs::UPDATE_WORLD_SEEN_BLINDAGENTS_ANGLES_ACTION_ID) {}

UpdateWorldSeenBlindAgentsAnglesAction::~UpdateWorldSeenBlindAgentsAnglesAction() { }

void UpdateWorldSeenBlindAgentsAnglesAction::prepare() {
	if(this->agent->getDatabase()->getDataItem("cellnextturnangle")) {
		local_index_map        = (MapIntIntData* ) this->agent->getDatabase()->getDataItem("local_index_map");
		nextTurnAngles   = (TVectorData<double>* ) this->agent->getDatabase()->getDataItem("cellnextturnangle");
		velocities       = (TVectorData<double>* ) this->agent->getDatabase()->getDataItem("cellspeeds");
	} else {
		velocities = 0;
		nextTurnAngles = 0;
		local_index_map = 0;
	}
}

void UpdateWorldSeenBlindAgentsAnglesAction::fire(Data *d) {
	if (this->agent == NULL)
		throw HiveException("Action was not assigned to an agent", "UpdateWorldSeenBlindAgentsAnglesAction(Data *d)");
	if (d->getType().compare("tvector_double")!=0)
		throw HiveException("wrong data passed to action", "UpdateWorldSeenBlindAgentsAnglesAction::fire(Data *d)");

	TVectorData<double> *argument = (TVectorData<double>* ) d;
	int cell_position             = (int)argument->at(0);
	double newangle				  = argument->at(1);
	double newvelocity			  = argument->at(2);

	int real_index                        = local_index_map->returnValueForKey(cell_position);
	nextTurnAngles->at(real_index)        = newangle;
	velocities->at(real_index)            = newvelocity;

//	cout<<"in world, recieving message from agent: "<<cell_position+1<<endl;
//	cout<<"real index is: "<<real_index<<" and new velocity is: "<<newvelocity<<endl;

}


UpdateWorldBackgroundConcentrationAction::UpdateWorldBackgroundConcentrationAction() :
	Action(ChemoPopActionIDs::UPDATE_WORLD_BACKGROUND_CONCENTRATION_ACTION_ID) { }

UpdateWorldBackgroundConcentrationAction::~UpdateWorldBackgroundConcentrationAction() { }

void UpdateWorldBackgroundConcentrationAction::prepare() {
	odlmfd = (OneDLinearMathFunctionData *) this->agent->getDatabase()->getDataItem("environment");
}

void UpdateWorldBackgroundConcentrationAction::fire(Data * d) {
	if (this->agent == NULL)
		throw HiveException("Action was not assigned to an agent", "UpdateWorldBackgroundConcentrationAction(Data *d)");
	if (d->getType().compare("DoubleType")!=0)
		throw HiveException("wrong data passed to action", "UpdateWorldBackgroundConcentrationAction::fire(Data *d)");

	double newvalue = ((DoubleData* ) d)->getDouble();

	// put out a warning when using this method!
	cerr << "# warning: executing UpdateWorldBackgroundConcentrationAction ... assuming that the environment in the " << endl
	     << "# world is of the type OneDLinearMathFunctionData. I have no idea what happens if it is of a different type. " << endl;
	odlmfd->setYIntercept(newvalue);
}


UpdateWorldDesiredConsumptionAction::UpdateWorldDesiredConsumptionAction():
	Action(ChemoPopActionIDs::UPDATE_WORLD_DESIRED_CONSUMPTION_ACTION_ID) { }

UpdateWorldDesiredConsumptionAction::~UpdateWorldDesiredConsumptionAction() { }

void UpdateWorldDesiredConsumptionAction::prepare() {
	desired_cell_consumption = (TVectorData<double>*) this->agent->getDatabase()->getDataItem("desired_cell_consumption");
	local_index_map                = (MapIntIntData*) this->agent->getDatabase()->getDataItem("local_index_map");
}

void UpdateWorldDesiredConsumptionAction::fire(Data *d) {
	if (this->agent == NULL)
		throw HiveException("Action was not assigned to an agent", "UpdateWorldDesiredConsumptionAction");
	if (d->getType().compare("tvector_double")!=0)
		throw HiveException("wrong data passed to action", "UpdateWorldDesiredConsumptionAction");

	TVectorData<double> *argument                 = (TVectorData<double> *) d;
	int cell_index                                = (int)argument->at(0);
	int real_index								  = local_index_map->returnValueForKey(cell_index);
	desired_cell_consumption->at(real_index)      = argument->at(1);

}




UpdateWorldBlindAgentDeathAction::UpdateWorldBlindAgentDeathAction():
		Action(ChemoPopActionIDs::UPDATE_WORLD_BLIND_AGENT_DEATH_ACTION_ID) { }

UpdateWorldBlindAgentDeathAction::~UpdateWorldBlindAgentDeathAction() {};

void UpdateWorldBlindAgentDeathAction::prepare() {
	local_index_map          = (MapIntIntData* ) this->agent->getDatabase()->getDataItem("local_index_map");
	index_to_agent_id_map    = (MapIntIntData* ) this->agent->getDatabase()->getDataItem("indexagentidmap");
	numberofcells            = (IntegerData *) this->agent->getDatabase()->getDataItem("numberofcells");
	cell_positions           = (TVectorData<TVectorData<double>* > *) this->agent->getDatabase()->getDataItem("cellpositions");
	cell_dir_vecs            = (TVectorData<TVectorData<double>* > *) this->agent->getDatabase()->getDataItem("celldirvecs");
	cellupvecs               = (TVectorData<TVectorData<double>* > *) this->agent->getDatabase()->getDataItem("cellupvecs");
	desired_cell_consumption = (TVectorData<double> *) this->agent->getDatabase()->getDataItem("desired_cell_consumption");
	cellspeeds               = (TVectorData<double> *) this->agent->getDatabase()->getDataItem("cellspeeds");


	this->existsCellVoxelPositions = agent->getDatabase()->existsDataItem("cell_voxel_positions");
	if(existsCellVoxelPositions) {
		cell_voxel_positions      = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("cell_voxel_positions");
		number_cells_per_voxel    = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("number_cells_per_voxel");
	}

	// if (celltype == ECOLI) {
	this->existsSwimmingStates = agent->getDatabase()->existsDataItem("swimmingstates");
	if(existsSwimmingStates) {
		swimmingstates          = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("swimmingstates");
		lastswimmingstates      = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("lastswimmingstates");
	}

	// if (celltype == BLIND) {
	this->existsCellNextTurnAngle = agent->getDatabase()->existsDataItem("cellnextturnangle");
	if(existsCellNextTurnAngle) {
		cellnextturnangle      = (TVectorData<double> *) this->agent->getDatabase()->getDataItem("cellnextturnangle");
		cell_wants_to_move     = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("cell_wants_to_move");
	}
}

void UpdateWorldBlindAgentDeathAction::fire(Data *d)
{

	//cerr<<"# UpdateWorldBlindAgentDeathAction::fire(Data *d) === handling death"<<endl;
	if (d->getType().compare("IntegerType")!=0)
		throw HiveException("wrong data passed to action", "UpdateWorldBlindAgentDeathAction");

	// (1) get the id of the deceased agent from the message
	int deadAgentId                         = ((IntegerData *) d)->getInteger();

	// (2) determine where the agent was located in the vectors
	//MapIntIntData *local_index_map          = (MapIntIntData* ) this->agent->getDatabase()->getDataItem("local_index_map");
	int real_index                          = local_index_map->returnValueForKey(deadAgentId);

	// (3) grab the map from indexes to agent_ids, which will also need updating
	//MapIntIntData *index_to_agent_id_map    = (MapIntIntData* ) this->agent->getDatabase()->getDataItem("indexagentidmap");

	// (4) grab the number of cells information from the database
	//IntegerData *numberofcells              = (IntegerData *) this->agent->getDatabase()->getDataItem("numberofcells");


	// Kill the cell through the registrar!
	// Send some final message to the special agent so it knows of the death
	Message *lastWords = new Message();
	lastWords->setAction(ChemoPopActionIDs::SPECIAL_AGENT_UPDATE_BLIND_AGENT_COUNT_ACTION_ID);
	lastWords->setArgument(new IntegerData("ChangeInBlindAgentNumber",-1));
	Registrar::getSystemRegistrar()->killAgentAndSendMessageToSpecialAgent(deadAgentId,lastWords);




	// (5) remove the deceased agent information from the maps (we have all the info we need for the rest)
	local_index_map->erase(deadAgentId);
	index_to_agent_id_map->erase(real_index);

	// (6) determine, based on the real index and current number of cells, if we have to swap this element
	//     to the last position or not.
	int lastIndex = numberofcells->getInteger()-1;
	if(real_index != lastIndex) {
		//if the real_index is not at the end, then we have to put it there with a swap
		//grab the AgentId that was at the end
		int swapped_AgentId = index_to_agent_id_map->returnValueForKey(lastIndex);

		//next, update the local_index_map to set the swapped agent position to the dead agent's position
		//and update the index_to_agent map so that the real_index points to the swapped agent
		local_index_map->set(swapped_AgentId,real_index);
		index_to_agent_id_map->set(real_index,swapped_AgentId);
		index_to_agent_id_map->erase(lastIndex);

		//the maps are up to date, but we still have to swap the data in all the vectors
		this->swap(real_index,lastIndex);
	}


	// (7) erase the last position in all the vectors and decrease the number of cells
	this->popLastElement();
	numberofcells->setInteger(numberofcells->getInteger()-1);


	// i think that is everything...
//	cerr<<"# world has been warned of a death, and has updated accordingly"<<endl;
	if( numberofcells->getInteger()==0 )
		cerr<<"# alas!  the final blind agent hath died."<<endl;
}


void UpdateWorldBlindAgentDeathAction::swap(int index1,int index2)
{

	// first, swap cell positions, directions, and up vectors
	//TVectorData<TVectorData<double>* > *cell_positions      = (TVectorData<TVectorData<double>* > *) this->agent->getDatabase()->getDataItem("cellpositions");
	TVectorData<double>* swap_space1 = cell_positions->at(index1);
	cell_positions->at(index1) = cell_positions->at(index2);
	cell_positions->at(index2) = swap_space1;

	//TVectorData<TVectorData<double>* > *cell_dir_vecs      = (TVectorData<TVectorData<double>* > *) this->agent->getDatabase()->getDataItem("celldirvecs");
	swap_space1 = cell_dir_vecs->at(index1);
	cell_dir_vecs->at(index1) = cell_dir_vecs->at(index2);
	cell_dir_vecs->at(index2) = swap_space1;

	//TVectorData<TVectorData<double>* > *cellupvecs      = (TVectorData<TVectorData<double>* > *) this->agent->getDatabase()->getDataItem("cellupvecs");
	swap_space1 = cellupvecs->at(index1);
	cellupvecs->at(index1) = cellupvecs->at(index2);
	cellupvecs->at(index2) = swap_space1;



	// second, update the data structure storing how much a cell wishes to consume
	// we will also use it to store the actual concentration that gets consumed by the cells
	// and the cell speeds

	//TVectorData<double> *desired_cell_consumption      = (TVectorData<double> *) this->agent->getDatabase()->getDataItem("desired_cell_consumption");
	double swap_space2 = desired_cell_consumption->at(index1);
	desired_cell_consumption->at(index1) = desired_cell_consumption->at(index2);
	desired_cell_consumption->at(index2) = swap_space2;

	//TVectorData<double> *cellspeeds      = (TVectorData<double> *) this->agent->getDatabase()->getDataItem("cellspeeds");
	swap_space2 = cellspeeds->at(index1);
	cellspeeds->at(index1) = cellspeeds->at(index2);
	cellspeeds->at(index2) = swap_space2;




	// if (environment == cube_grid) {
	if(this->existsCellVoxelPositions) {
		//TVectorData<int> *cell_voxel_positions      = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("cell_voxel_positions");
		int swap_space3 = cell_voxel_positions->at(index1);
		cell_voxel_positions->at(index1) = cell_voxel_positions->at(index2);
		cell_voxel_positions->at(index2) = swap_space3;
	}



	// if (celltype == ECOLI) {
	if(this->existsSwimmingStates) {
		//TVectorData<int> *swimmingstates      = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("swimmingstates");
		int swap_space3 = swimmingstates->at(index1);
		swimmingstates->at(index1) = swimmingstates->at(index2);
		swimmingstates->at(index2) = swap_space3;

		//TVectorData<int> *lastswimmingstates      = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("lastswimmingstates");
		swap_space3 = lastswimmingstates->at(index1);
		lastswimmingstates->at(index1) = lastswimmingstates->at(index2);
		lastswimmingstates->at(index2) = swap_space3;
	}



	// if (celltype == BLIND) {
	if(this->existsCellNextTurnAngle) {

		// add data object needed by the blind searcher, which is the
		// next angle to use during turns.  Default to zero and must be
		// overridden by the agent
		//TVectorData<double> *cellnextturnangle      = (TVectorData<double> *) this->agent->getDatabase()->getDataItem("cellnextturnangle");
		double swap_space4 = cellnextturnangle->at(index1);
		cellnextturnangle->at(index1) = cellnextturnangle->at(index2);
		cellnextturnangle->at(index2) = swap_space4;

		//TVectorData<int> *cell_wants_to_move       = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("cell_wants_to_move");
		int swap_space5 = cell_wants_to_move->at(index1);
		cell_wants_to_move->at(index1) = cell_wants_to_move->at(index2);
		cell_wants_to_move->at(index2) = swap_space5;
	}
}

void UpdateWorldBlindAgentDeathAction::popLastElement()
{
	// first, swap cell positions, directions, and up vectors
	//TVectorData<TVectorData<double>* > *cell_positions      = (TVectorData<TVectorData<double>* > *) this->agent->getDatabase()->getDataItem("cellpositions");
	delete cell_positions->at(cell_positions->size()-1);
	cell_positions->removeElementFromEnd();

	//TVectorData<TVectorData<double>* > *cell_dir_vecs      = (TVectorData<TVectorData<double>* > *) this->agent->getDatabase()->getDataItem("celldirvecs");
	delete cell_dir_vecs->at(cell_dir_vecs->size()-1);
	cell_dir_vecs->removeElementFromEnd();


	//TVectorData<TVectorData<double>* > *cellupvecs      = (TVectorData<TVectorData<double>* > *) this->agent->getDatabase()->getDataItem("cellupvecs");

	///  MEMORY LEAK IF WE DON'T DELETE TVECTORDATA PTR
	delete cellupvecs->at(cellupvecs->size()-1);
	cellupvecs->removeElementFromEnd();



	// second, update the data structure storing how much a cell wishes to consume
	// we will also use it to store the actual concentration that gets consumed by the cells
	// and the cell speeds

	//TVectorData<double> *desired_cell_consumption      = (TVectorData<double> *) this->agent->getDatabase()->getDataItem("desired_cell_consumption");
	desired_cell_consumption->removeElementFromEnd();

	//TVectorData<double> *cellspeeds      = (TVectorData<double> *) this->agent->getDatabase()->getDataItem("cellspeeds");
	cellspeeds->removeElementFromEnd();




	// if (environment == cube_grid) {
	if(this->existsCellVoxelPositions) {
		//TVectorData<int> *cell_voxel_positions      = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("cell_voxel_positions");
		int cellVoxelPositionForLastCell = cell_voxel_positions->at(cell_voxel_positions->size()-1);
		cell_voxel_positions->removeElementFromEnd();

		// and remove that cell from the correct voxel position
		//TVectorData<int> *number_cells_per_voxel      = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("number_cells_per_voxel");
		number_cells_per_voxel->at(cellVoxelPositionForLastCell) =
				number_cells_per_voxel->at(cellVoxelPositionForLastCell) - 1;
	}



	// if (celltype == ECOLI) {
	if(this->existsSwimmingStates) {
		//TVectorData<int> *swimmingstates      = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("swimmingstates");
		swimmingstates->removeElementFromEnd();

		//TVectorData<int> *lastswimmingstates      = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("lastswimmingstates");
		lastswimmingstates->removeElementFromEnd();
	}



	// if (celltype == BLIND) {
	if(this->existsCellNextTurnAngle) {

		// add data object needed by the blind searcher, which is the
		// next angle to use during turns.  Default to zero and must be
		// overridden by the agent
		//TVectorData<double> *cellnextturnangle      = (TVectorData<double> *) this->agent->getDatabase()->getDataItem("cellnextturnangle");
		cellnextturnangle->removeElementFromEnd();


		//TVectorData<int> *cell_wants_to_move       = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("cell_wants_to_move");
		cell_wants_to_move->removeElementFromEnd();
	}



}




UpdateWorldBlindAgentBirthAction::UpdateWorldBlindAgentBirthAction():
		Action(ChemoPopActionIDs::UPDATE_WORLD_BLIND_AGENT_BIRTH_ACTION_ID) { }

UpdateWorldBlindAgentBirthAction::~UpdateWorldBlindAgentBirthAction() {};

void UpdateWorldBlindAgentBirthAction::prepare() {
	local_index_map          = (MapIntIntData* ) this->agent->getDatabase()->getDataItem("local_index_map");
	index_to_agent_id_map    = (MapIntIntData* ) this->agent->getDatabase()->getDataItem("indexagentidmap");
	numberofcells            = (IntegerData *) this->agent->getDatabase()->getDataItem("numberofcells");
	cell_positions           = (TVectorData<TVectorData<double>* > *) this->agent->getDatabase()->getDataItem("cellpositions");
	cell_dir_vecs            = (TVectorData<TVectorData<double>* > *) this->agent->getDatabase()->getDataItem("celldirvecs");
	cellupvecs               = (TVectorData<TVectorData<double>* > *) this->agent->getDatabase()->getDataItem("cellupvecs");
	desired_cell_consumption = (TVectorData<double> *) this->agent->getDatabase()->getDataItem("desired_cell_consumption");
	cellspeeds               = (TVectorData<double> *) this->agent->getDatabase()->getDataItem("cellspeeds");

	this->existsCellVoxelPositions = agent->getDatabase()->existsDataItem("cell_voxel_positions");
	if(existsCellVoxelPositions) {
		cell_voxel_positions      = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("cell_voxel_positions");
		number_cells_per_voxel    = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("number_cells_per_voxel");
	}

	// if (celltype == ECOLI) {
	this->existsSwimmingStates = agent->getDatabase()->existsDataItem("swimmingstates");
	if(existsSwimmingStates) {
		swimmingstates          = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("swimmingstates");
		lastswimmingstates      = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("lastswimmingstates");
	}

	// if (celltype == BLIND) {
	this->existsCellNextTurnAngle = agent->getDatabase()->existsDataItem("cellnextturnangle");
	if(existsCellNextTurnAngle) {
		cellnextturnangle      = (TVectorData<double> *) this->agent->getDatabase()->getDataItem("cellnextturnangle");
		cell_wants_to_move     = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("cell_wants_to_move");
	}
}

void UpdateWorldBlindAgentBirthAction::fire(Data *d)
{
	if (d->getType().compare("tvectordata_int")!=0)
		throw HiveException("wrong data passed to action", "UpdateWorldBlindAgentBirthAction");

	// parse the new information
	TVectorData<int> *argument      = (TVectorData<int> *) d;
	int motherAgentID = argument->at(0);    // in the first position, the agent that gave birth to it
	int babyAgentID = argument->at(1);  // in the second position, is the new agent ID


	// grab the data we need to begin with
	//MapIntIntData *local_index_map          = (MapIntIntData* ) this->agent->getDatabase()->getDataItem("local_index_map");
	//MapIntIntData *index_to_agent_id_map    = (MapIntIntData* ) this->agent->getDatabase()->getDataItem("indexagentidmap");
	//IntegerData *numberofcells              = (IntegerData *) this->agent->getDatabase()->getDataItem("numberofcells");

	// add the baby to the maps and increase the number of cells
	// the baby will always be added to the last position in the vectors
	local_index_map->insert(babyAgentID,numberofcells->getInteger());
	index_to_agent_id_map->insert(numberofcells->getInteger(),babyAgentID);
	numberofcells->setInteger(numberofcells->getInteger()+1);

	// get the mother agent index, so we can look up information about where to place and set up the baby
	int motherLocalIndex = local_index_map->returnValueForKey(motherAgentID);


	// first, init the cell positions, directions, and up vectors
	//TVectorData<TVectorData<double>* > *cell_positions      = (TVectorData<TVectorData<double>* > *) this->agent->getDatabase()->
	//                                                                   getDataItem("cellpositions");

	//TVectorData<TVectorData<double>* > *cell_dir_vecs      = (TVectorData<TVectorData<double>* > *) this->agent->getDatabase()->
    //                                                                   getDataItem("celldirvecs");

	//TVectorData<TVectorData<double>* > *cellupvecs      = (TVectorData<TVectorData<double>* > *) this->agent->getDatabase()->
    //                                                                   getDataItem("cellupvecs");

	// next, create the new position information and initalize them
	TVectorData<double>* babyPosition = new TVectorData<double> ("position", "tvectordata_double");
	babyPosition->reserveSize(3);
	// the position is the same as the mothers
	babyPosition->at(0) = cell_positions->at(motherLocalIndex)->at(0);
	babyPosition->at(1) = cell_positions->at(motherLocalIndex)->at(1);
	babyPosition->at(2) = cell_positions->at(motherLocalIndex)->at(2);
	cell_positions->addElementToEnd(babyPosition);

	TVectorData<double>* babyDirection = new TVectorData<double> ("direction", "tvectordata_double");
	babyDirection->reserveSize(3);
	cell_dir_vecs->addElementToEnd(babyDirection);

	TVectorData<double>* babyUp = new TVectorData<double> ("up", "tvectordata_double");
	babyUp->reserveSize(3);
	cellupvecs->addElementToEnd(babyUp);


	// the position is the same as the mothers
//	for(int k=0; k<cell_positions->at(motherLocalIndex)->size(); k++)
//		babyPosition->addElementToEnd(cell_positions->at(motherLocalIndex)->at(k));

	// direction and up have to be random
	double dir[3]; dir[0]=1; dir[1]=0; dir[2]=0;
	double up[3]; up[0]=0; up[1]=1; up[2]=0;
	double rotMat[3][3];
	rotMat[0][0] = 1;	rotMat[0][1] = 0;	rotMat[0][2] = 0;
	rotMat[1][0] = 0;	rotMat[1][1] = 1; 	rotMat[1][2] = 0;
	rotMat[2][0] = 0;	rotMat[2][1] = 0; 	rotMat[2][2] = 1;

	Util::genUniformRandRotation3d(rotMat);
	Util::applyRotation(rotMat,dir);
	Util::applyRotation(rotMat,up);

	babyDirection->at(0) = dir[0]; babyDirection->at(1) = dir[1]; babyDirection->at(2) = dir[2];
	babyUp->at(0) = up[0]; babyUp->at(1) = up[1]; babyUp->at(2) = up[2];

//	cerr<<" birthing new baby"<<endl;
//	cerr<<"  dir:"<<dir[0]<<" "<<dir[1]<<" "<<dir[2]<<endl;
//	cerr<<"  up:"<<up[0]<<" "<<up[1]<<" "<<up[2]<<endl;
//	cerr<<"   dir:"<<babyDirection->at(0)<<" "<< babyDirection->at(1) <<" "<< babyDirection->at(2);
//	cerr<<"   up:"<<babyUp->at(0)<<" "<< babyUp->at(1) <<" "<< babyUp->at(2);


	// set the consumption and speed to be the same as the parent
	//TVectorData<double> *desired_cell_consumption      = (TVectorData<double> *) this->agent->getDatabase()->getDataItem("desired_cell_consumption");
	desired_cell_consumption->addElementToEnd(desired_cell_consumption->at(motherLocalIndex));

	//TVectorData<double> *cellspeeds      = (TVectorData<double> *) this->agent->getDatabase()->getDataItem("cellspeeds");
	cellspeeds->addElementToEnd(cellspeeds->at(motherLocalIndex));

	// if (environment == cube_grid) {
	if(this->existsCellVoxelPositions) {
		//TVectorData<int> *cell_voxel_positions      = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("cell_voxel_positions");
		int motherVoxelPosition = cell_voxel_positions->at(motherLocalIndex);
		cell_voxel_positions->addElementToEnd(motherVoxelPosition);

		//add the new cell to the correct voxel
		//TVectorData<int> *number_cells_per_voxel      = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("number_cells_per_voxel");
		number_cells_per_voxel->at(motherVoxelPosition) = number_cells_per_voxel->at(motherVoxelPosition)+1;
	}

	// if (celltype == ECOLI) {
	if(this->existsSwimmingStates) {
		//TVectorData<int> *swimmingstates      = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("swimmingstates");
		swimmingstates->addElementToEnd(swimmingstates->at(motherLocalIndex));

		//TVectorData<int> *lastswimmingstates      = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("lastswimmingstates");
		lastswimmingstates->addElementToEnd(lastswimmingstates->at(motherLocalIndex));
	}

	// if (celltype == BLIND) {
	if(this->existsCellNextTurnAngle) {
		// add data object needed by the blind searcher, which is the
		// next angle to use during turns.  Default to zero and must be
		// overridden by the agent
		//TVectorData<double> *cellnextturnangle      = (TVectorData<double> *) this->agent->getDatabase()->getDataItem("cellnextturnangle");
		cellnextturnangle->addElementToEnd(cellnextturnangle->at(motherLocalIndex));

		//TVectorData<int> *cell_wants_to_move       = (TVectorData<int> *) this->agent->getDatabase()->getDataItem("cell_wants_to_move");
		cell_wants_to_move->addElementToEnd(cell_wants_to_move->at(motherLocalIndex));
	}
	// add the baby to the maps and increase the number of cells
	// cerr<<"# world has been warned of a birth, and has updated accordingly"<<endl;

}


UpdateCellSeenLigandAction::UpdateCellSeenLigandAction() :
	Action(ChemoPopActionIDs::UPDATE_CELL_SEEN_LIGAND_ACTION_ID) { }
UpdateCellSeenLigandAction::~UpdateCellSeenLigandAction() { }

void UpdateCellSeenLigandAction::prepare() {
	Database *db      = agent->getDatabase();
	essence           = (DoubleData*) db->getDataItem("essence");
	kcat              = (DoubleData*) db->getDataItem("kcat");
	Km                = (DoubleData*) db->getDataItem("Km");
	appetite          = (TVectorData<double>* ) db->getDataItem("appetite");
	ligand_conc       = (TVectorData<double>* ) db->getDataItem("ligands");
	yield             = (DoubleData*) db->getDataItem("yield");
	effeciency        = (DoubleData*) db->getDataItem("base_effeciency_of_conversion");
}

void UpdateCellSeenLigandAction::fire(Data *d) {
	if (this->agent == NULL)
		throw HiveException("Action was not assigned to an agent", "UpdateCellSeenLigandAction(Data *d)");
	if (d->getType().compare("tvector_double")!=0)
		throw HiveException("wrong data passed to action", "UpdateCellSeenLigandAction::fire(Data *d)");


	// The cell responds to seen ligand here.  It does this by setting its local ligand concentration
	// and by setting its appetite (which is its rate of uptake, and is based on Km and kcat)

	// first grab the argument
	TVectorData<double> *argument          = (TVectorData<double>* ) d;

	// update the seen ligand value
	for(unsigned int i=0; i<ligand_conc->size(); i++)
		ligand_conc->at(i) = argument->at(i);

	// determine the uptake rate (the appetite) for ligand zero
	int lig_index = 0;
	appetite->at(lig_index) = effeciency->getDouble()*
			(1.0/yield->getDouble())*essence->getDouble() * (
			(kcat->getDouble()*ligand_conc->at(lig_index)) /
			(Km->getDouble() + ligand_conc->at(lig_index))  );

	if(appetite->at(lig_index)<=0)  appetite->at(lig_index)=0;
	//cerr<<agent->getAgentId()<<" seen:" << ligand_conc->at(0)<<endl;
	//cerr<<agent->getAgentId()<<" want:" << effeciency->getDouble()*
	//		essence->getDouble() * (
	//		(kcat->getDouble()*ligand_conc->at(lig_index)) /
	//		(Km->getDouble() + ligand_conc->at(lig_index))  )<<endl;
	//cerr<<agent->getAgentId()<<" ask :" << appetite->at(lig_index)<<endl;
}


UpdateCellAccumulatedLigandAction::UpdateCellAccumulatedLigandAction() :
	Action(ChemoPopActionIDs::UPDATE_CELL_ACCUMULATED_LIGAND_ACTION_ID) { }

UpdateCellAccumulatedLigandAction::~UpdateCellAccumulatedLigandAction() {}

void UpdateCellAccumulatedLigandAction::prepare() {
	Database *db         = agent->getDatabase();
	essence              = (DoubleData*) db->getDataItem("essence");
	yield				 = (DoubleData*) db->getDataItem("yield");
	effeciency        = (DoubleData*) db->getDataItem("base_effeciency_of_conversion");
//	accumulated_ligands  = (TVectorData<double>* ) db->getDataItem("accumulated_ligands");
}

void UpdateCellAccumulatedLigandAction::fire(Data *d) {
	if (this->agent == NULL)
		throw HiveException("Action was not assigned to an agent", "UpdateCellAccumulatedLigandAction(Data*d)");
	if (d->getType().compare("DoubleType")!=0)
		throw HiveException("wrong data passed to action", "UpdateCellSeenLigandAction::fire(Data *d)");

	DoubleData *argument = (DoubleData* ) d;
	if(argument->getDouble()<=0) return;

	// this directly updates the accumulated ligands data structure. that way it is ensured
	// that the consumption of the last iteration is incorporated before the simulator is envoked

	// take whatever the world gave us, and turn it into essence.  again, we do this here so
	// that we ensure future operations act on the updated essence
	//  NOTE:::  THIS WAS ORIGINALLY DIVIDE BY YIELD, WHICH IS WRONG!
	essence->setDouble(essence->getDouble() +
			(1/effeciency->getDouble())*(argument->getDouble()*yield->getDouble())     );
	//cerr<<agent->getAgentId()<<" rec :" << argument->getDouble()<<endl;
	//cerr<<agent->getAgentId()<<" gain:" << argument->getDouble()*yield->getDouble()<<endl;

}


SpecialAgentReceiveCellPositionsAction::SpecialAgentReceiveCellPositionsAction():
	Action(ChemoPopActionIDs::SPECIAL_AGENT_RECEIVE_CELL_POSITIONS_ACTION_ID)  {
	this->isbinary = false;
}

SpecialAgentReceiveCellPositionsAction::~SpecialAgentReceiveCellPositionsAction() {
	this->out->close();
	delete out;
}

void SpecialAgentReceiveCellPositionsAction::prepare() {

}

void SpecialAgentReceiveCellPositionsAction::fire(Data *d) {
	if (this->agent == NULL)
		throw HiveException("Action was not assigned to an agent", "SpecialAgentReceiveCellPositionsAction(Data *d)");
	if (d->getType().compare("tvector_doublematrix")!=0)
		throw HiveException("wrong data passed to action", "SpecialAgentReceiveCellPositionsAction::fire(Data *d)");

	// recieve the data
	TVectorData<TVectorData<double>* > *positions = (TVectorData<TVectorData<double>* >*) d;

	// first element is always the time (in a vector of length one)
	double time = ((TVectorData<double> *)positions->at(0))->at(0);

	// nothing to do with the data other than generating the output.
	if (!this->isbinary) {
		//*this->out << "# output at time t=" << ((SpecialAgent*) this->agent)->getInternalTime()  << endl;
		//*this->out  <<time << " " << 0 << " " << 0	<<" "<< 0 << endl;
		for (int i=1; i<positions->size(); i++)
			*this->out  <<time<<" "<<positions->at(i)->at(0) << " " << positions->at(i)->at(1) << " " << positions->at(i)->at(2)
				<<" "<< positions->at(i)->at(3) << endl;
		//*this->out << endl << endl;
	} else {
		double d;
		for (int i=0; i<positions->size(); i++) {
			d = positions->at(i)->at(0);
			this->out->write(reinterpret_cast<char *> (&d), sizeof(d));
			d = positions->at(i)->at(1);
			this->out->write(reinterpret_cast<char *> (&d), sizeof(d));
			d = positions->at(i)->at(2);
			this->out->write(reinterpret_cast<char *> (&d), sizeof(d));
			d = positions->at(i)->at(3);
			this->out->write(reinterpret_cast<char *> (&d), sizeof(d));
		}
	}
}

void SpecialAgentReceiveCellPositionsAction::prepareOutputfile(string name, bool bin) {
	if (bin) {
		this->out = new ofstream(name.c_str(),ios_base::binary);
		this->isbinary = true;
	}
	else
		this->out = new ofstream(name.c_str());
}

SpecialAgentAccumulateCellDataAction::SpecialAgentAccumulateCellDataAction() :
	Action(ChemoPopActionIDs::SPECIAL_AGENT_ACCUMULATE_CELL_DATA_ACTION_ID) {
	this->cellcounter = 0;
	this->isbinary    = false;
}

SpecialAgentAccumulateCellDataAction::~SpecialAgentAccumulateCellDataAction() {
	this->out->close();
	delete out;
}

void SpecialAgentAccumulateCellDataAction::prepare() {
	cell_matrix = (TVectorData<TVectorData<double>* >* ) agent->getDatabase()->getDataItem("celldatamatrix");
	numberofcellsData =(IntegerData *)agent->getDatabase()->getDataItem("numberofcells");
}

void SpecialAgentAccumulateCellDataAction::fire(Data *d) {
	if (this->agent == NULL)
		throw HiveException("Action was not assigned to an agent", "SpecialAgentAccumulateCellDataAction(Data *d)");
	if (d->getType().compare("tvector_double")!=0)
		throw HiveException("wrong data passed to action", "SpecialAgentAccumulateCellDataAction::fire(Data *d)");

	// convert the received data
	TVectorData<double> *received_info = (TVectorData<double> *) d;



	// copy the data into the right position
	// needs to be done ...
	int index = received_info->at(0);
	for (int i=1; i<received_info->size(); i++)
		cell_matrix->at(index-1)->at(i-1) = received_info->at(i);

	this->cellcounter += 1;
	// do the output if all cells are added.
	if (cellcounter == (numberofcellsData)->getInteger() ) {
		// do the output
		if (!this->isbinary) {
			*this->out << "# output at time t=" << ((SpecialAgent*) this->agent)->getInternalTime()  << endl;
			for (int i=0; i<(numberofcellsData)->getInteger(); i++) {
				for (int j=0; j<cell_matrix->at(i)->size(); j++)
					*this->out << cell_matrix->at(i)->at(j) << " ";
				*this->out << endl;
			}
			this->cellcounter = 0;
			*this->out << endl << endl;
		} else {
			double d;
			for (int i=0; i<(numberofcellsData)->getInteger(); i++) {
				for (int j=0; j<cell_matrix->at(i)->size(); j++) {
					double d = cell_matrix->at(i)->at(j);
					this->out->write(reinterpret_cast<char *> (&d), sizeof(double));
				}
			}
			this->cellcounter = 0;
		}
	}

}

void SpecialAgentAccumulateCellDataAction::prepareOutputfile(string filename, bool bin) {
	if (bin) {
		this->out = new ofstream(filename.c_str(),ios_base::binary);
		this->isbinary = true;
	}
	else
		this->out = new ofstream(filename.c_str());
}

SpecialAgentAccumulateBlindAgentDataAction::SpecialAgentAccumulateBlindAgentDataAction():
Action(ChemoPopActionIDs::SPECIAL_AGENT_ACCUMULATE_BLINDAGENT_DATA_ACTION_ID) {
	this->blindagentcounter = 0;
	this->isbinary = false;
}

SpecialAgentAccumulateBlindAgentDataAction::~SpecialAgentAccumulateBlindAgentDataAction() {
	this->out->close();
	delete out;
}

void SpecialAgentAccumulateBlindAgentDataAction::prepare() {

}

void SpecialAgentAccumulateBlindAgentDataAction::fire(Data* d) {
	if (this->agent == NULL)
		throw HiveException("Action was not assigned to an agent", "SpecialAgentAccumulateBlindAgentDataAction(Data *d)");
	if (d->getType().compare("tvector_double")!=0)
		throw HiveException("wrong data passed to action", "SpecialAgentAccumulateBlindAgentDataAction::fire(Data *d)");

	// convert the received data
	TVectorData<double> *received_info = (TVectorData<double> *) d;

	// we directly output the output

	if (this->isbinary) {
		cerr << "# binary output for specialAgentAccumulateBlindAgentDataAction::fire has not yet been implemented" << endl;
	} else {
		// output the agent id
		*out << (int)received_info->at(0) << " ";
		int number_letters = (int)received_info->at(1);
		for (int i=2; i<2+number_letters; i++)
			*out << (char)((int)received_info->at(i));
		*out << " ";
		for (int j=number_letters+2; j<received_info->size(); j++)
			*out << received_info->at(j) << " ";
		*out << endl;
	}


	/*

	Database *db = agent->getDatabase();
	// get the relevant data item from the database
	TVectorData<TVectorData<double>* > *blind_agent_matrix =
		(TVectorData<TVectorData<double>* >* ) db->getDataItem("blindagentdatamatrix");

	// copy the data into the right position
	// needs to be done ...
	int index = (int)received_info->at(0);

	//	cerr<<"setting: blind_agent_matrix->at("<<blindagentcounter<<")"<<endl;
	//	cerr<<"sizes: "<<blind_agent_matrix->size()<<endl;
	//In theory, index == agentID..
	//cerr << "SpecialAgentAccumulateBlindAgentDataAction::fire(Data* d)  ====  receiving agent data from: "<<index<<endl;
	//cerr << blindagentcounter << " " << ((IntegerData *) db->getDataItem("numberofblindagents"))->getInteger() << endl;
	for (int i=0; i<received_info->size(); i++) {
		blind_agent_matrix->at(blindagentcounter)->at(i) = received_info->at(i);
	}
	//cerr<<"setting: blind_agent_matrix->at("<<blindagentcounter<<")"<<endl;
	//cerr<<"sizes: "<<blind_agent_matrix->size()<<endl;
	//cerr<<"sizes: "<<blind_agent_matrix->at(blindagentcounter)->size()<<endl;

	this->blindagentcounter += 1;
	int n_blind_agents = ((IntegerData *) db->getDataItem("numberofblindagents"))->getInteger();
	//cerr<<" n_blind_agents = "<<n_blind_agents<<endl;
	if (blindagentcounter == n_blind_agents ) {

		//cerr<<"SpecialAgentAccumulateBlindAgentDataAction::fire(Data* d): === is dumping "<<endl;
		// do the output
		if (!this->isbinary) {
			*this->out << "# output at time t=" << ((SpecialAgent*) this->agent)->getInternalTime()  << endl;
			for (int i=0; i<n_blind_agents; i++) {
				for (int j=0; j<blind_agent_matrix->at(i)->size(); j++)
					*this->out << blind_agent_matrix->at(i)->at(j) << " ";
				*this->out << endl;
			}
			*this->out << endl << endl;
		} else {
			double d;
			for (int i=0; i<n_blind_agents; i++) {
				for (int j=0; j<blind_agent_matrix->at(i)->size(); j++) {
					double d = blind_agent_matrix->at(i)->at(j);
					this->out->write(reinterpret_cast<char *> (&d), sizeof(double));
				}
			}
		}

		//reset the blind agent counter for this step
		this->blindagentcounter = 0;

		// determine how many agents died or were born during that last step
		IntegerData *changeInBlindAgentNumberSinceLastOutput = (IntegerData *) db->getDataItem("change_in_blind_agent_number_since_last_output");


		// add the new or dead agents to our count
		int new_blind_agent_count = n_blind_agents+changeInBlindAgentNumberSinceLastOutput->getInteger();
		((IntegerData *) db->getDataItem("numberofblindagents"))->setInteger(new_blind_agent_count);

		//reset the number of of agents that changed over the past step
		changeInBlindAgentNumberSinceLastOutput->setInteger(0);


		//cerr<<"blind_agent_matrix->size() = "<< blind_agent_matrix->size();


		// increase the size of the blind agent matrix if needed to get the blind agent matrix
		// to be the same size as the new number of agents
		TVectorData<TVectorData<double>* > *blind_agent_matrix =
				(TVectorData<TVectorData<double>* >* ) db->getDataItem("blindagentdatamatrix");
		//cerr<<"blind_agent_matrix->size() = "<< blind_agent_matrix->size()<<endl;
		//cerr<<"new_blind_agent_count- = " << new_blind_agent_count<<endl;

		for(int k=blind_agent_matrix->size(); k<=new_blind_agent_count; k++)
		{
			TVectorData<double> *newmatrixelement = new TVectorData<double>;
			newmatrixelement->reserveSize(blind_agent_matrix->at(0)->size());
			blind_agent_matrix->addElementToEnd(newmatrixelement);
		}

		//cerr << "SpecialAgentAccumulateBlindAgentDataAction::fire(Data* d)  ====  dumping output"<<endl;
		//exit(1);

	}
	*/
}

void SpecialAgentAccumulateBlindAgentDataAction::prepareOutputfile(string filename, bool bin) {
	if (bin) {
		this->out = new ofstream(filename.c_str(),ios_base::binary);
		this->isbinary = true;
	} else
		this->out = new ofstream(filename.c_str());
}

SpecialAgentUpdateBlindAgentCountAction::SpecialAgentUpdateBlindAgentCountAction():
Action(ChemoPopActionIDs::SPECIAL_AGENT_UPDATE_BLIND_AGENT_COUNT_ACTION_ID) {
}

SpecialAgentUpdateBlindAgentCountAction::~SpecialAgentUpdateBlindAgentCountAction() { }

void SpecialAgentUpdateBlindAgentCountAction::prepare() {

}

void SpecialAgentUpdateBlindAgentCountAction::fire(Data* d) {
	if (this->agent == NULL)
		throw HiveException("Action was not assigned to an agent", "SpecialAgentUpdateCellCountAction(Data *d)");
	if (d->getType().compare("IntegerType")!=0)
		throw HiveException("wrong data passed to action", "SpecialAgentUpdateCellCountAction::fire(Data *d)");

	// convert the received data
	IntegerData *received_info = (IntegerData *) d;

	// grab the database
	Database *db = agent->getDatabase();

	// update the number of agents
	int currentNumber = ((IntegerData *) db->getDataItem("change_in_blind_agent_number_since_last_output"))->getInteger();
	currentNumber += received_info->getInteger();
	((IntegerData *) db->getDataItem("change_in_blind_agent_number_since_last_output"))->setInteger(currentNumber);


	//cerr<<"SpecialAgentUpdateBlindAgentCountAction::fire() === Updating blind agent count to: "<<currentNumber<<endl;

}

SpecialAgentOutputGridAction::SpecialAgentOutputGridAction():
Action(ChemoPopActionIDs::SPECIAL_AGENT_OUTPUT_GRID_ACTION_ID) {
	this->isbinary = false;
	this->out = NULL;
}

SpecialAgentOutputGridAction::~SpecialAgentOutputGridAction() {
	this->out->close();
	delete out;
}

void SpecialAgentOutputGridAction::prepare() {

}

void SpecialAgentOutputGridAction::prepareOutputfile(string filename, bool bin) {
	if (bin) {
		this->out = new ofstream(filename.c_str(), ios_base::binary);
		this->isbinary = true;
	} else
		this->out = new ofstream(filename.c_str());
}

void SpecialAgentOutputGridAction::fire(Data* d) {
	if (this->agent == NULL)
		throw HiveException("Action was not assigned to an agent", "SpecialAgentOutputGridAction(Data *d)");
	if (d->getType().compare("tvector_double")!=0)
		throw HiveException("wrong data passed to action", "SpecialAgentOutputGridAction::fire(Data *d)");

	TVectorData<double> *arg = (TVectorData<double> *) d;
	if (!this->isbinary) {
		for (int i=1; i<=arg->size(); i++) {
			*out << arg->at(i-1) << " ";
			if (i%4 == 0)
				*out << endl;
		}
		*out << endl << endl;
	} else {
		for (int i=0; i<arg->size(); i++) {
			double d = arg->at(i);
			this->out->write(reinterpret_cast<char *> (&d), sizeof(double));
		}
	}
}









