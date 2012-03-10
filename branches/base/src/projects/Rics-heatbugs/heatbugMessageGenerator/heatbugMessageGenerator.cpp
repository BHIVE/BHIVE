#include "heatbugMessageGenerator.hh"

using namespace std;
using namespace Hive;
using namespace Heatbug;

// assigning values to the heatbug action ids
const int HeatbugActionIDs::WORLD_UPDATE_DESIRED_MOVEMENT_OF_BUG_ACTION_ID = 0;
const int HeatbugActionIDs::WORLD_UPDATE_TEMPERATURE_PRODUCED_BY_BUG_ACTION_ID = 1;
const int HeatbugActionIDs::BUG_UPDATE_SEEN_TEMPERATURE_ACTION_ID = 0;
const int HeatbugActionIDs::SPECIAL_AGENT_RECEIVE_BUG_POSITIONS_ACTION_ID = 0;
const int HeatbugActionIDs::SPECIAL_AGENT_RECEIVE_TEMPERATURE_FIELD_ACTION_ID = 1;
const int HeatbugActionIDs::SPECIAL_AGENT_UPDATE_BUG_HAPPINESS_ACTION_ID = 2;


WorldNotifyBugAboutLocalTemperatureMessageGenerator::WorldNotifyBugAboutLocalTemperatureMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db = source->getDatabase();
	this->temperature_grid = (TVectorData<TVectorData<DoubleData>* >*) db->getDataItem("temperature_grid");
	this->bug_positions = (TVectorData<TVectorData<IntegerData>* > *) db->getDataItem("bug_positions");
	this->grid_size = ((IntegerData *) db->getDataItem("grid_size"))->getInteger();
}

WorldNotifyBugAboutLocalTemperatureMessageGenerator::~WorldNotifyBugAboutLocalTemperatureMessageGenerator() { }

void WorldNotifyBugAboutLocalTemperatureMessageGenerator::placeMessage(int destID) {
	// set up a new message
	Message *msg = new Message();
	msg->setAction(HeatbugActionIDs::BUG_UPDATE_SEEN_TEMPERATURE_ACTION_ID);
	msg->setDestinationID(destID);

	// calculate local index of bug
	int bug_index = destID-1;

	TVectorData<double> *arg = new TVectorData<double> ("argument", "tvector_doubledata");
	int x = bug_positions->at(bug_index)->at(0).getInteger();
	int y = bug_positions->at(bug_index)->at(1).getInteger();

	// using periodic boundaries here
	double t       = temperature_grid->at(x)->at(y).getDouble();
	double t_up    = temperature_grid->at(x)->at((y+1)%grid_size).getDouble();
	double t_right = temperature_grid->at((x+1)%grid_size)->at(y).getDouble();
	double t_down  = temperature_grid->at(x)->at((y-1+grid_size)%grid_size).getDouble();
	double t_left  = temperature_grid->at((x-1+grid_size)%grid_size)->at(y).getDouble();


	// add data to argument vector
	arg->addElementToEnd(t);
	arg->addElementToEnd(t_up);
	arg->addElementToEnd(t_right);
	arg->addElementToEnd(t_down);
	arg->addElementToEnd(t_left);

	// finalise message
	msg->setArgument(arg);
	source->placeMessageInOutbox(msg);
}


BugNotifyWorldAboutDesiredMovementMessageGenerator::BugNotifyWorldAboutDesiredMovementMessageGenerator(Agent *ag) : MessageGenerator(ag, true) {
	this->db = source->getDatabase();
	this->move_direction = (IntegerData *) db->getDataItem("move_direction");
}

BugNotifyWorldAboutDesiredMovementMessageGenerator::~BugNotifyWorldAboutDesiredMovementMessageGenerator() { }

void BugNotifyWorldAboutDesiredMovementMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(HeatbugActionIDs::WORLD_UPDATE_DESIRED_MOVEMENT_OF_BUG_ACTION_ID);
	msg->setDestinationID(destID);

	TVectorData<int> *arg = new TVectorData<int> ("argument", "tvector_int");
	arg->reserveSize(2);

	arg->at(0) = source->getAgentId();
	arg->at(1) = move_direction->getInteger();

	msg->setArgument(arg);
	source->placeMessageInOutbox(msg);
}


BugNotifyWorldAboutProducedHeatMessageGenerator::BugNotifyWorldAboutProducedHeatMessageGenerator(Agent *ag) : MessageGenerator(ag, true) {
	this->db = source->getDatabase();
	this->heat_rate = (DoubleData* ) db->getDataItem("heat_rate");
}

BugNotifyWorldAboutProducedHeatMessageGenerator::~BugNotifyWorldAboutProducedHeatMessageGenerator() { }

void BugNotifyWorldAboutProducedHeatMessageGenerator::placeMessage(int destID) {
	Message *msg = new Message();
	msg->setAction(HeatbugActionIDs::WORLD_UPDATE_TEMPERATURE_PRODUCED_BY_BUG_ACTION_ID);
	msg->setDestinationID(destID);

	TVectorData<double> *arg = new TVectorData<double>("argument", "tvector_double");
	arg->reserveSize(2);

	arg->at(0) = (double) source->getAgentId();
	arg->at(1) = heat_rate->getDouble();

	msg->setArgument(arg);
	source->placeMessageInOutbox(msg);
}


WorldTellSpecialAgentAboutBugPositionsMessageGenerator::WorldTellSpecialAgentAboutBugPositionsMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db = source->getDatabase();
	this->bug_grid = (TVectorData<TVectorData<IntegerData>* >*) db->getDataItem("bug_grid");
	this->time = (DoubleData *) db->getDataItem("worldtime");
	this->noti = (DoubleData *) db->getDataItem("next_output_time");
}

WorldTellSpecialAgentAboutBugPositionsMessageGenerator::~WorldTellSpecialAgentAboutBugPositionsMessageGenerator() { }

void WorldTellSpecialAgentAboutBugPositionsMessageGenerator::placeMessage(int destID) {
	if (this->time->getDouble() >= this->noti->getDouble()) {
		Message *msg = new Message();
		msg->setAction(HeatbugActionIDs::SPECIAL_AGENT_RECEIVE_BUG_POSITIONS_ACTION_ID);
		msg->setDestinationID(destID);

		// copy bug grid into argument
		TVectorData<TVectorData<IntegerData>* > *arg = new TVectorData<TVectorData<IntegerData>* > ("","");
		arg->reserveSize(bug_grid->size());
		for (int i=0; i<bug_grid->size(); i++) {
			arg->at(i) = new TVectorData<IntegerData> ("","");
			arg->at(i)->reserveSize(bug_grid->at(i)->size());
			for (int j=0; j<bug_grid->at(i)->size(); j++)
				arg->at(i)->at(j).setInteger(bug_grid->at(i)->at(j).getInteger());
		}

		msg->setArgument(arg);
		source->placeMessageInOutbox(msg);
	}
}


WorldTellSpecialAgentAboutTemperatureGridMessageGenerator::WorldTellSpecialAgentAboutTemperatureGridMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db = source->getDatabase();
	this->temperature_grid = (TVectorData<TVectorData<DoubleData>* > *) db->getDataItem("temperature_grid");

	this->time = (DoubleData *) db->getDataItem("worldtime");
	this->noti = (DoubleData *) db->getDataItem("next_output_time");
	this->oi   = (DoubleData *) db->getDataItem("output_interval");
}

WorldTellSpecialAgentAboutTemperatureGridMessageGenerator::~WorldTellSpecialAgentAboutTemperatureGridMessageGenerator() { }

void WorldTellSpecialAgentAboutTemperatureGridMessageGenerator::placeMessage(int destID) {
	if (this->time->getDouble() >= this->noti->getDouble()) {
		Message *msg = new Message();
		msg->setAction(HeatbugActionIDs::SPECIAL_AGENT_RECEIVE_TEMPERATURE_FIELD_ACTION_ID);
		msg->setDestinationID(destID);

		// copy temperature grid into argument
		TVectorData<TVectorData<DoubleData>* > *arg = new TVectorData<TVectorData<DoubleData>* > ("","");
		arg->reserveSize(temperature_grid->size());
		for (int i=0; i<temperature_grid->size(); i++) {
			arg->at(i) = new TVectorData<DoubleData>("","");
			arg->at(i)->reserveSize(temperature_grid->at(i)->size());
			for (int j=0; j<temperature_grid->at(i)->size(); j++)
				arg->at(i)->at(j).setDouble(temperature_grid->at(i)->at(j).getDouble());
		}

		msg->setArgument(arg);
		source->placeMessageInOutbox(msg);

		this->noti->setDouble(this->noti->getDouble() + this->oi->getDouble());
	}
}

BugTellSpecialAgentAboutHappinessMessageGenerator::BugTellSpecialAgentAboutHappinessMessageGenerator(Agent *ag) : MessageGenerator(ag, false) {
	this->db = source->getDatabase();
	this->local_temperature = (TVectorData<DoubleData> *) db->getDataItem("local_temperature_around_me");
	this->preferred_temperature = (DoubleData *) db->getDataItem("preferred_temperature");

	this->time = (DoubleData *) db->getDataItem("bug_time");
	this->noti = (DoubleData *) db->getDataItem("next_output_time");
	this->oi   = (DoubleData *) db->getDataItem("output_interval");
}

BugTellSpecialAgentAboutHappinessMessageGenerator::~BugTellSpecialAgentAboutHappinessMessageGenerator() { }

void BugTellSpecialAgentAboutHappinessMessageGenerator::placeMessage(int destID) {
	if (this->time->getDouble() >= this->noti->getDouble() ) {
		Message *msg = new Message();
		msg->setAction(HeatbugActionIDs::SPECIAL_AGENT_UPDATE_BUG_HAPPINESS_ACTION_ID);
		msg->setDestinationID(destID);

		TVectorData<double> *argument = new TVectorData<double>("argument", "tvector_double");
		argument->reserveSize(1);
		// happiness: (real_temperature - prefered_temperature) / prefered_temperature;
		argument->at(0) = (local_temperature->at(0).getDouble() - preferred_temperature->getDouble())/preferred_temperature->getDouble();

		msg->setArgument(argument);
		source->placeMessageInOutbox(msg);

		// update the time of the next output
		this->noti->setDouble(this->noti->getDouble() + this->oi->getDouble());
	}
}
