/**
 * @file transwellAction.cpp
 *
 * this file contains the definitions of the actions
 *
 * @author garrit jentsch
 * @data jan 14, 2010
 */

#include "transwellAction.hh"

using namespace Hive;
using namespace Transwell;

// world actions
InitWorldCellPositionAction::InitWorldCellPositionAction() :
	Action(TranswellActionIDs::INIT_WORLD_CELL_POSITION_ACTION_ID) {}

InitWorldCellPositionAction::~InitWorldCellPositionAction() {}

void InitWorldCellPositionAction::fire(Data *d) {
//	cerr << "# inside init world cell position" << endl;
	if (this->agent == NULL)
		throw HiveException("Action was not assigned to an agent","InitWorldCellPositionAction::fire(Data *d)");
	if (d->getType().compare("tvectordata_double")!=0) {
		cerr << d->getType() << endl;
		throw HiveException("wrong data passed to action", "InitWorldCellPositionAction::fire(Data *d)");
	}
	// convert data into correct format
	TVectorData<double> *pos = (TVectorData<double> *) d;
	// get the relevant information from the
	Grid *g = (Grid *) agent->getDatabase()->getDataItem("world_agent_grid");
	TVectorData<int> *pvp = (TVectorData<int> *) agent->getDatabase()->getDataItem("particles_voxel_positions");

	// get the cellid
	int source = (int)pos->at(0)-1;
	// calculate voxel position
	int voxel  = g->insideVoxel(pos->at(1),pos->at(2), pos->at(3));
	// set entry
	pvp->setEntry(source,voxel);
//	pvp->at(source) = voxel;

}

UpdateWorldSeenIL2Action::UpdateWorldSeenIL2Action() :
	Action(TranswellActionIDs::UPDATE_WORLD_SEEN_IL2_ACTION_ID) {}

UpdateWorldSeenIL2Action::~UpdateWorldSeenIL2Action() {}

void UpdateWorldSeenIL2Action::fire(Data *d) {
//	cerr << "# inside updateworldseenil2action" << endl;
	if (this->agent == NULL)
		throw HiveException("action was not assigned to an agent", "UpdateWorldSeenIL2Action::fire(Data *d)");
	if (d->getType().compare("tvectordata_double")!=0)
		throw HiveException("wrong data passed to action", "UpdateWorldSeenIL2Action::fire(Data *d)");

	// convert data argument into correct type
	TVectorData<double> *il2 = (TVectorData<double>* ) d;
	// get the relevant data from the agent's database
	Grid *g = (Grid *) agent->getDatabase()->getDataItem("world_agent_grid");
	TVectorData<int> *pvp = (TVectorData<int> *) agent->getDatabase()->getDataItem("particles_voxel_positions");
	int source = (int)il2->at(0)-1;
	int voxel  = pvp->at(source);
	// calculate il2 difference
//	double delta = g->getVoxel(voxel)->getOldConcentration(0) - il2->at(1);
	// add difference to il2 concentration
	g->getVoxel(voxel)->setConcentration(0, il2->at(1)); // g->getVoxel(voxel)->getOldConcentration(0)-delta);
//	g->getVoxel(voxel)->setConcentration(0,il2->at(1));
	if (g->getVoxel(voxel)->getConcentration(0) < 0)
		throw HiveException("il2 number smaller zero", "UpdateWorldSeenIL2Action::fire(Data *d)");
}

// cell actions
UpdateCellSeenIL2Action::UpdateCellSeenIL2Action() :
	Action(TranswellActionIDs::UPDATE_CELL_SEEN_IL2_ACTION_ID) {}

UpdateCellSeenIL2Action::~UpdateCellSeenIL2Action() {}

void UpdateCellSeenIL2Action::fire(Data *d) {
//	cerr << "# inside updatecellseenil2action" << endl;
	if (this->agent == NULL)
		throw HiveException("action was not assigned to an agent", "UpdateCellSeenIL2Action::fire(Data *d)");
	if (d->getType().compare("DoubleType")!=0)
		throw HiveException("wrong data passed to action","UpdateCellSeenIL2Action::fire(Data *d)");

	// convert data argument into correct type
	DoubleData *il2 = (DoubleData *) d;
	// get the relevant data from the agent's database
	TVectorData<double> *conc = (TVectorData<double> *) agent->getDatabase()->getDataItem("concentrations");
	// set to new value
	conc->at(0) = il2->getDouble();

}

UpdateCellSeenParticlesAction::UpdateCellSeenParticlesAction() :
	Action(TranswellActionIDs::UPDATE_CELL_SEEN_PARTICLES_ACTION_ID) {}

UpdateCellSeenParticlesAction::~UpdateCellSeenParticlesAction() {}

void UpdateCellSeenParticlesAction::fire(Data *d) {
//	cerr << "# inside updatecellseenparticlesaction" << endl;
	if (this->agent == NULL)
		throw HiveException("action was not assigned to an agent", "UpdateCellSeenParticlesAction::fire(Data *d)");
	if (d->getType().compare("tvectordata_int")!=0)
		throw HiveException("wrong data passed to action", "UpdateCellSeenParticlesAction::fire(Data *d)");

	// convert data argument into correct type
	TVectorData<int> *part = (TVectorData<int> *) d;
	// get the relevant data from the agent's database
	TVectorData<double> *conc = (TVectorData<double> *) agent->getDatabase()->getDataItem("concentrations");

	// NOTE THAT WE KNOW WHERE WHICH PARTICLES ARE LOCATED WITHIN THE AGENTS. THERE MUST BE A WAY TO
	// DO THIS INDEPENDENT OF THAT KNOWLEDGE ...
	// update the stat5 numbers; in the nucleus, stat5 is only produced by unbinding from the chromosome.
	conc->at(5) = conc->at(5)+part->at(0);
	// set the pstat5 numbers
	conc->at(6) = part->at(1);
	// set the chrom_il2 numbers
	conc->at(7) = part->at(2);
	conc->at(8) = part->at(3);
	conc->at(9) = part->at(4);
	// set the chrom_reca numbers
	conc->at(10) = part->at(5);
	conc->at(11) = part->at(6);
	// set the chrom_recb numbers
	conc->at(12) = part->at(8);
	conc->at(13) = part->at(9);
}


// nucleus action
UpdateNucleusSeenPSTAT5Action::UpdateNucleusSeenPSTAT5Action() :
	Action(TranswellActionIDs::UPDATE_NUCLEUS_SEEN_PSTAT5_ACTION_ID) {}

UpdateNucleusSeenPSTAT5Action::~UpdateNucleusSeenPSTAT5Action() {}

void UpdateNucleusSeenPSTAT5Action::fire(Data *d) {
//	cerr << "# inside updateNulceusSeenpstat5" << endl;
	//cerr << d->getType() << endl;
	if (this->agent == NULL)
		throw HiveException("action was not assigned to an agent", "UpdateNucleusSeenPSTAT5Action::fire(Data *d)");
	if (d->getType().compare("IntegerType")!=0)
		throw HiveException("wrong data passed to action", "UpdateNucleusSeenPSTAT5Action::fire(Data *d)");

	// convert data argument into correct type
	IntegerData *pstat5 = (IntegerData *) d;
	// get the relevant data from the agent's database
	TVectorData<int> *part = (TVectorData<int> *) agent->getDatabase()->getDataItem("particle_numbers");
	// set pstat5 in the agent's particle vector to the correct value
	// NOTE WE KNOW THAT PSTAT5 IS STORED AT POSITION 1 IN THE NUCLEUS'S PARTICELVECTOR
	// set the pstat5 number to the given value
	part->at(1) = pstat5->getInteger();
	// set the stat5 number to zero
	part->at(0) = 0;
}

SpecialAgentAccumulateDataAction::SpecialAgentAccumulateDataAction() :
	Action(TranswellActionIDs::SPECIAL_AGENT_ACCUMULATE_CELL_DATA_ACTION_ID) {
	out = NULL;
}

SpecialAgentAccumulateDataAction::~SpecialAgentAccumulateDataAction() {
	delete out;
}

void SpecialAgentAccumulateDataAction::fire(Data *d) {
//	cerr << "# inside specialAgentAccumulatedata" << endl;
	if (this->agent == NULL)
		throw HiveException("action was not assigned to an agent", "SpecialAgentAccumulateDataAction::fire(Data *d)");
	if (d->getType().compare("tvectordata_double")!=0)
		throw HiveException("wrong data passed to action", "SpecialAgentAccumulateDataAction::fire(Data *d)");

	//	cerr << "INSIDE SPECIALAGENT FIRE" << endl;
	// convert data argument into correct format
	TVectorData<double> *c = (TVectorData<double> *) d;

	// get the databases from the agent ... do we really need the meta database ?! not really
	// one could do without ... i just thought it would be funny to have a meta database in a special agent
	// called fox mulder ... but i guess that we should remove this ...
	Database *meta = ((SpecialAgent *) agent)->getMetaDatabase();
	Database *db   = agent->getDatabase();
	// get some meta information
	IntegerData *num_adds =  (IntegerData* ) meta->getDataItem("number_of_adds");
	IntegerData *num_cells = (IntegerData* ) meta->getDataItem("total_number_of_cells");
	IntegerData *num_time_steps_between_outputs = (IntegerData* ) meta->getDataItem("number_between_outputs");
	IntegerData *counter = (IntegerData* ) meta->getDataItem("counter");

	// accumulate the data
	if (num_adds->getInteger() == 0) {
		// fill data
		TVectorData<double> *conc = (TVectorData<double> *) db->getDataItem("total_conc");
		for (int i=1; i<c->size(); i++)
			conc->at(i-1) = c->at(i);
	} else {
		TVectorData<double> *conc = (TVectorData<double>*) db->getDataItem("total_conc");
		for (int i=1; i<c->size(); i++)
			conc->at(i-1) = conc->at(i-1) + c->at(i);
	}
	num_adds->setInteger(num_adds->getInteger()+1);
	// check whether output has to be generated
	if (num_adds->getInteger() == num_cells->getInteger() &&
		counter->getInteger() == num_time_steps_between_outputs->getInteger()-1) {
	//	cerr << "JETZT BIN ICH HIER DIRIN UND SOLLTE ETWAS AUSGEBEN!!!" << endl;
		TVectorData<double> *conc = (TVectorData<double>*) db->getDataItem("total_conc");
		DoubleData *gt = (DoubleData* ) meta->getDataItem("global_time");
		DoubleData *dt = (DoubleData* ) meta->getDataItem("time_step");
		// output time;
		*this->out << setw(12) << setfill(' ') << gt->getDouble()*num_time_steps_between_outputs->getInteger();
		// update time
		gt->setDouble(gt->getDouble() + dt->getDouble());
		// output conc vector
		for (int i=0; i<conc->size(); i++)
			*this->out << setw(12) << setfill(' ') << conc->at(i)/num_cells->getInteger();
		*this->out << endl;
		// reset num_adds
		num_adds->setInteger(0);
		// reset counter
		counter->setInteger(0);
	} else if (num_adds->getInteger() == num_cells->getInteger()){
	//	cerr << "hier drin" << endl;
		num_adds->setInteger(0);
		counter->setInteger(counter->getInteger()+1);
	}

}

void SpecialAgentAccumulateDataAction::prepareOutputfile(string name) {
	this->out = new ofstream(name.c_str());
}

