#include "heatbugAction.hh"

using namespace Hive;
using namespace Heatbug;

WorldUpdateDesiredMovementOfBugAction::WorldUpdateDesiredMovementOfBugAction() :
		Action(HeatbugActionIDs::WORLD_UPDATE_DESIRED_MOVEMENT_OF_BUG_ACTION_ID) {
	this->agent = NULL;
}

WorldUpdateDesiredMovementOfBugAction::~WorldUpdateDesiredMovementOfBugAction() { }

void WorldUpdateDesiredMovementOfBugAction::fire(Data *d) {
	// cast the argument into the correct type
	TVectorData<int> *arg = (TVectorData<int> *) d;

	// get the position of the bug
	int bugID       = arg->at(0);
	int local_bugID = bugID-1;
	int direct      = arg->at(1);
	int x           = bug_positions->at(local_bugID)->at(0).getInteger();
	int y           = bug_positions->at(local_bugID)->at(1).getInteger();

	// calculate the new position based on the desired movement
	if (direct == 0) { // stay home
		x+=0; y+=0;
	} else if (direct == 1) {
		y = (y+1)%gridsize;
	} else if (direct == 2) {
		x = (x+1)%gridsize;
	} else if (direct == 3) {
		y = (y-1+gridsize)%gridsize;
	} else if (direct == 4) {
		x = (x-1+gridsize)%gridsize;
	} else {
		throw HiveException("wrong direction passed to method", "void WorldUpdateDesiredMovementOfBugAction::fire(Data *d)");
	}

	// add agent to desired new position
	desired_bug_grid->at(x)->at(y)->addElementToEnd(IntegerData("bugId",bugID));
	desired_bug_positions->at(local_bugID)->at(0).setInteger(x);
	desired_bug_positions->at(local_bugID)->at(1).setInteger(y);

}

void WorldUpdateDesiredMovementOfBugAction::prepare() {
	if (this->agent == NULL)
		throw HiveException("#agent not assigned", "in class WorldUpdateMovementOfBugAction");
	else {
		Database *db                = this->agent->getDatabase();
		this->desired_bug_grid      = (TVectorData<TVectorData<TVectorData<IntegerData>* >* >*) db->getDataItem("desired_bug_grid");
		this->desired_bug_positions = (TVectorData<TVectorData<IntegerData>* >* ) db->getDataItem("desired_bug_positions");
		this->bug_positions         = (TVectorData<TVectorData<IntegerData>* >*) db->getDataItem("bug_positions");
		this->gridsize              = ((IntegerData*) db->getDataItem("grid_size"))->getInteger();
	}
}


WorldUpdateTemperatureProducedByBugAction::WorldUpdateTemperatureProducedByBugAction() :
		Action(HeatbugActionIDs::WORLD_UPDATE_TEMPERATURE_PRODUCED_BY_BUG_ACTION_ID) {
	this->agent = NULL;
}

WorldUpdateTemperatureProducedByBugAction::~WorldUpdateTemperatureProducedByBugAction() { }

void WorldUpdateTemperatureProducedByBugAction::fire(Data *d) {
	// cast argument to correct type
	TVectorData<double> *arg = (TVectorData<double> *) d;
	// get information from the argument
	int bugID       = (int) arg->at(0);
	int local_bugID = bugID -1;
	double rate     = arg->at(1);

	int x           = bug_positions->at(local_bugID)->at(0).getInteger();
	int y           = bug_positions->at(local_bugID)->at(1).getInteger();

	// calculate the change in temperature at that place
	double dtemp = rate*this->dt;
	//cerr << this->dt << endl;
//	exit(1);

	// update temperature
	temperature_grid->at(x)->at(y).addToValue(dtemp);
}

void WorldUpdateTemperatureProducedByBugAction::prepare() {
	if (this->agent == NULL)
		throw HiveException("#agent not assigned", "in class WorldUpdateTemperatureProducedBugAction");
	else {
		Database *db           = this->agent->getDatabase();
		this->temperature_grid = (TVectorData<TVectorData<DoubleData>* >*) db->getDataItem("temperature_grid");
		this->bug_positions    = (TVectorData<TVectorData<IntegerData>* > *) db->getDataItem("bug_positions");
		this->dt               = ((DoubleData*) db->getDataItem("dt"))->getDouble();
	}
}


BugUpdateSeenTemperatureAction::BugUpdateSeenTemperatureAction() :
		Action(HeatbugActionIDs::BUG_UPDATE_SEEN_TEMPERATURE_ACTION_ID) {
	this->agent = NULL;
}

BugUpdateSeenTemperatureAction::~BugUpdateSeenTemperatureAction() { }

void BugUpdateSeenTemperatureAction::fire(Data *d) {
	// cast argument to correct type
	TVectorData<double> *arg = (TVectorData<double> *) d;

	// simply copy content to correct location
	for (int i=0; i<arg->size(); i++)
		local_temperature_around_me->at(i).setDouble(arg->at(i));
}

void BugUpdateSeenTemperatureAction::prepare() {
	if (this->agent == NULL)
		throw HiveException("# no agent assigned to action", " in class BugUpdateSeenTemperatureAction");
	else {
		Database *db                      = this->agent->getDatabase();
		this->local_temperature_around_me = (TVectorData<DoubleData> *) db->getDataItem("local_temperature_around_me");
	}
}


SpecialAgentReceiveBugPositionsAction::SpecialAgentReceiveBugPositionsAction() :
		Action(HeatbugActionIDs::SPECIAL_AGENT_RECEIVE_BUG_POSITIONS_ACTION_ID) {
	this->agent = NULL;
}

SpecialAgentReceiveBugPositionsAction::~SpecialAgentReceiveBugPositionsAction() { }

void SpecialAgentReceiveBugPositionsAction::fire(Data *d) {
	// convert the argument to the correct type
	TVectorData<TVectorData<IntegerData>* > *local_bug_grid =
			(TVectorData<TVectorData<IntegerData>* >*) d;

	// do the output in the correct format
	if (!isbinary) {
		for (int i=0; i<local_bug_grid->size(); i++) {
			for (int j=0; j<local_bug_grid->at(i)->size(); j++)
				*this->out << local_bug_grid->at(i)->at(j).getInteger() << " ";
			*this->out << endl;
		}
		*this->out << endl << endl;
	} else {
		int d;
		for (int i=0; i<local_bug_grid->size(); i++)
			for (int j=0; j<local_bug_grid->at(i)->size(); j++) {
				d = local_bug_grid->at(i)->at(j).getInteger();
				this->out->write(reinterpret_cast<char *> (&d), sizeof(d));
			}
	}
}

void SpecialAgentReceiveBugPositionsAction::prepare() {}

void SpecialAgentReceiveBugPositionsAction::prepareOutputfile(string name, bool bina) {
	if (bina == true) {
		this->out = new ofstream(name.c_str(),ios_base::binary);
		this->isbinary = true;
	} else {
		this->out = new ofstream(name.c_str());
		this->isbinary = false;
	}
}


SpecialAgentReceiveTemperatureFieldAction::SpecialAgentReceiveTemperatureFieldAction() :
		Action(HeatbugActionIDs::SPECIAL_AGENT_RECEIVE_TEMPERATURE_FIELD_ACTION_ID) {
	this->agent = NULL;
}

SpecialAgentReceiveTemperatureFieldAction::~SpecialAgentReceiveTemperatureFieldAction() { }

void SpecialAgentReceiveTemperatureFieldAction::fire(Data *d) {
	// convert the argument to the correct type
	TVectorData<TVectorData<DoubleData>* > *local_temp_grid =
			(TVectorData<TVectorData<DoubleData>* >*) d;

	// do the output in the correct format
	if (!isbinary) {
		for (int i=0; i<local_temp_grid->size(); i++) {
			for (int j=0; j<local_temp_grid->at(i)->size(); j++)
				*out << local_temp_grid->at(i)->at(j).getDouble() << " ";
				*out << endl;
		}
		*out << endl << endl;
	} else {
		double d;
		for (int i=0; i<local_temp_grid->size(); i++)
			for (int j=0; j<local_temp_grid->at(i)->size(); j++) {
				d = local_temp_grid->at(i)->at(j).getDouble();
				this->out->write(reinterpret_cast<char *> (&d), sizeof(d));
			}
	}
}

void SpecialAgentReceiveTemperatureFieldAction::prepareOutputfile(string name, bool bina) {
	if (bina == true) {
		this->out = new ofstream(name.c_str(),ios_base::binary);
		this->isbinary = true;
	} else {
		this->out = new ofstream(name.c_str());
		this->isbinary = false;
	}
}

void SpecialAgentReceiveTemperatureFieldAction::prepare() {}


SpecialAgentUpdateBugHappinessAction::SpecialAgentUpdateBugHappinessAction() :
		Action(HeatbugActionIDs::SPECIAL_AGENT_UPDATE_BUG_HAPPINESS_ACTION_ID) {
	this->agent           = NULL;
	this->number_received = 0;
	this->number_bugs     = 0;
	this->output_interval = 0;
	this->acc_happiness   = 0;
	this->number_outputs  = 0;
}

SpecialAgentUpdateBugHappinessAction::~SpecialAgentUpdateBugHappinessAction() {
}

void SpecialAgentUpdateBugHappinessAction::fire(Data *d) {
	// convert the argument to correct type
	TVectorData<double> *arg = (TVectorData<double> *) d;

	// get the happiness
	double happiness = arg->at(0);

	// accumulated the happiness
	this->acc_happiness   += happiness;
	this->number_received += 1;

	// if applicable, do the output
	if (this->number_received == this->number_bugs) {
		if (!isbinary)
			outfile << this->number_outputs*this->output_interval << " " << this->acc_happiness/this->number_bugs << endl;
		else if (isbinary) {
			// due to some strange reason, it is not writing to out!
			double dummy(1.0);
			dummy = this->number_outputs*this->output_interval;
			outfile.write(reinterpret_cast<char*>(&dummy), sizeof(dummy));
			dummy = this->acc_happiness/this->number_bugs;
			outfile.write(reinterpret_cast<char*>(&dummy), sizeof(dummy));
		}
		// update bookkeeping variables
		this->number_received = 0;
		this->acc_happiness   = 0;
		this->number_outputs += 1;
	}


}

void SpecialAgentUpdateBugHappinessAction::prepare() {
	Database *db          = agent->getDatabase();
	this->number_bugs     = ((IntegerData *) db->getDataItem("number_bugs"))->getInteger();
	this->output_interval = ((DoubleData *) db->getDataItem("output_interval"))->getDouble();
}

void SpecialAgentUpdateBugHappinessAction::prepareOutputfile(string name, bool bina) {
	// no matter what the flag, we always do a non-binary output. i do not get it to work
	// in binary mode. do not ask we why! it is a complete mistery to me.
	outfile.open(name.c_str());
	isbinary = false;

/*
	if (bina == true) {
		outfile.open(name.c_str(),ios::binary);
		isbinary = true;
	} else {
		outfile.open(name.c_str()); //this->out = new ofstream(name.c_str());
		isbinary = false;
	}
*/
}




