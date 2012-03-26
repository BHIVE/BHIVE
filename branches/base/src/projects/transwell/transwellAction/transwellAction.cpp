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


	// <------------ THIS PART ONLY NEEDS TO BE UNCOMMENTED FOR THE TRANSWELL SIMULATIONS. -----------------------
	// essentially: by having this we ignore the placement decission of the cell and put it 
	// to a new position. 
	IntegerData *ca = (IntegerData*) agent->getDatabase()->getDataItem("cellsadded");
	ca->setInteger(ca->getInteger()+1);
	// this needs to be set to the correct cell number in the simulation
	if (ca->getInteger() == 782) {
	  //	  cerr << "entering first" << endl;
	          map<int, int> lookup;
	          int number = 0;
	          // determine how many voxels form the surface
	          for (int i=0; i<g->getNumberOfVoxels(); i++)
	        	  // this needs to be set to the correct height!
	        	  if (g->getCenterZ(i) < 0.05)
	        		  number += 1;
	          // fill up pvp vector
	          for (int i=0; i<pvp->size(); i++) {
	        	  do {
	        		  pvp->at(i) = (int)Util::RANDOM(number-1);
	        	  } while (lookup[pvp->at(i)] > 10); // this needs to be set to the maximal number of cells per voxel that we will allow
	        	  lookup[pvp->at(i)] += 1;
	          }
		  // now we write to a log-file in which we indicate at which center coordinate a cell sits. 
		  ofstream tcp("tcell_positions.dat");
		  for (int i=0; i<pvp->size(); i++)
		    tcp << i << " " << g->getCenterX(pvp->at(i)) 
		  	<< " " <<  g->getCenterY(pvp->at(i))  
			<< " " << g->getCenterZ(pvp->at(i)) << endl;
		  tcp.close();
	}

}



InitWorldTregPositionAction::InitWorldTregPositionAction() :
	Action(TranswellActionIDs::INIT_WORLD_TREG_POSITION_ACTION_ID) {}

InitWorldTregPositionAction::~InitWorldTregPositionAction() {}

void InitWorldTregPositionAction::fire(Data *d) {
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

	// <------------ THIS PART ONLY NEEDS TO BE UNCOMMENTED FOR THE TRANSWELL SIMULATIONS. -----------------------
	IntegerData *ca = (IntegerData*) agent->getDatabase()->getDataItem("tregsadded");
	ca->setInteger(ca->getInteger()+1);
       
	// this needs to be set to the correct cell number in the simulation
	if (ca->getInteger() == 782) {
	  // the domain lookup is only needed if we run simulations with a transwell geometry. 
	  // TVectorData<int> *dl = (TVectorData<int>*) agent->getDatabase()->getDataItem("domain_lookup");

	  //	  cerr << "entering loop" << endl;
	          map<int, int> lookup;
	          int number = 0;
	          // determine how many and which voxels form the surface of the inner well
	          // <-------------- THE INNER-WELL-HEIGHT NEEDS TO BE SET BY HAND !!!
	          // double inner_well_height = 0.4;
		  double lower_height = 0.0;
		  double upper_height = 0.05;
	          vector<int> inner_well_surface_indices;
	          for (int i=0; i<g->getNumberOfVoxels(); i++) {
		    //  cerr << g->getVoxel(i)->getDomain() << endl;
		    //  if (dl->at(i) == 1 && g->getCenterZ(i) < inner_well_height) {
		    if (g->getCenterZ(i) > lower_height && g->getCenterZ(i) < upper_height) {
	        		  number += 1;
	        		  inner_well_surface_indices.push_back(i);
	        	  }
		  }
	          // fill up pvp vector
	          for (int i=782; i<pvp->size(); i++) {
	        	  do {
	        		  pvp->at(i) = inner_well_surface_indices[(int)Util::RANDOM(number-1)];
	        	  } while (lookup[pvp->at(i)] > 10); // this needs to be set to the maximal number of cells per voxel that we will allow
	        	  lookup[pvp->at(i)] += 1;
	          }
		  // now we write to a log-file in which we indicate at which center coordinate a cell sits. 
		  ofstream tcp("tregcell_positions.dat");
		  for (int i=0; i<pvp->size(); i++)
		    tcp << i << " " << g->getCenterX(pvp->at(i)) 
		  	<< " " <<  g->getCenterY(pvp->at(i))  
			<< " " << g->getCenterZ(pvp->at(i)) << endl;
		  tcp.close();
	}
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
//	cerr << "received time stemp " << il2->at(2) << " " << ((DoubleData*)this->agent->getDatabase()->getDataItem("propagate_until"))->getDouble() << endl;
	// only change the il2 concentration if the message was send at the end of the time step!
	if (il2->at(2) == ((DoubleData*) this->agent->getDatabase()->getDataItem("propagate_until"))->getDouble()) {
		// get the relevant data from the agent's database
		Grid *g = (Grid *) agent->getDatabase()->getDataItem("world_agent_grid");
		TVectorData<int> *pvp = (TVectorData<int> *) agent->getDatabase()->getDataItem("particles_voxel_positions");
		int source = (int)il2->at(0)-1;
		int voxel  = pvp->at(source);
		// calculate il2 difference
		double delta = g->getVoxel(voxel)->getOldConcentration(0) - il2->at(1);
//		cerr << il2->at(2) << " " << delta << endl;
		// add difference to il2 concentration
		g->getVoxel(voxel)->setConcentration(0, g->getVoxel(voxel)->getConcentration(0)-delta);
//		cerr << "world set to: " << g->getVoxel(voxel)->getConcentration(0) << endl;
		//	g->getVoxel(voxel)->setConcentration(0,il2->at(1));
		if (g->getVoxel(voxel)->getConcentration(0) < 0)
			throw HiveException("il2 number smaller zero", "UpdateWorldSeenIL2Action::fire(Data *d)");
	}
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
//	cerr << "tcell receives " << il2->getDouble() << endl;
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
	TVectorData<double> *part = (TVectorData<double> *) d;
	// get the relevant data from the agent's database
//	TVectorData<double> *conc = (TVectorData<double> *) agent->getDatabase()->getDataItem("concentrations");

	TVectorData<double> *remember = (TVectorData<double>*)agent->getDatabase()->getDataItem("remember");
	if (remember->size() == 0)
		remember->reserveSize(part->size());

	for (int i=0; i<part->size(); i++)
		remember->at(i) = part->at(i);

	((BoolData*) agent->getDatabase()->getDataItem("event_occured"))->setBool(true);

	// NOTE THAT WE KNOW WHERE WHICH PARTICLES ARE LOCATED WITHIN THE AGENTS. THERE MUST BE A WAY TO
	// DO THIS INDEPENDENT OF THAT KNOWLEDGE ...
	// update the stat5 numbers; in the nucleus, stat5 is only produced by unbinding from the chromosome.
	/*
	conc->at(5) = conc->at(5)+part->at(1);
	// set the pstat5 numbers
	conc->at(6) = part->at(2);
	// set the chrom_il2 numbers
	conc->at(7) = part->at(3);
	conc->at(8) = part->at(4);
	conc->at(9) = part->at(5);
	// set the chrom_reca numbers
	conc->at(10) = part->at(6);
	conc->at(11) = part->at(7);
	// set the chrom_recb numbers
	conc->at(12) = part->at(9);
	conc->at(13) = part->at(10);
*/
	// update the time until which the agent is allowed to propagate
	((DoubleData*)agent->getDatabase()->getDataItem("local_propagate_until"))->setDouble(part->at(0));
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
	if (d->getType().compare("tvectordata_double")!=0)
		throw HiveException("wrong data passed to action", "UpdateNucleusSeenPSTAT5Action::fire(Data *d)");

	// convert data argument into correct type
	TVectorData<double> *pstat5 = (TVectorData<double> *) d;
	// get the relevant data from the agent's database
	TVectorData<int> *part = (TVectorData<int> *) agent->getDatabase()->getDataItem("particle_numbers");
	// set pstat5 in the agent's particle vector to the correct value
	// NOTE WE KNOW THAT PSTAT5 IS STORED AT POSITION 1 IN THE NUCLEUS'S PARTICELVECTOR
	// set the pstat5 number to the given value
	part->at(1) = ((int) pstat5->at(0));
	// set the stat5 number to zero
	part->at(0) = 0;
	// set the time of the agent
	((DoubleData *) agent->getDatabase()->getDataItem("time"))->setDouble(pstat5->at(1));
	((DoubleData *) agent->getDatabase()->getDataItem("delta"))->setDouble(pstat5->at(0));
	((DoubleData *) agent->getDatabase()->getDataItem("propagate_until"))->setDouble(pstat5->at(2));
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
	//	if (d->getType().compare("tvectordata_double")!=0) 
	//  	cerr << d->getType() << endl;
	if (d->getType().compare("tvectordata_double")!=0)
		throw HiveException("wrong data passed to action", "SpecialAgentAccumulateDataAction::fire(Data *d)");
	
	// cerr << "INSIDE SPECIALAGENT FIRE" << endl;
	// convert data argument into correct format
	TVectorData<double> *arg = (TVectorData<double> *) d;

	// get the databases from the agent ... do we really need the meta database ?! not really
	// one could do without ... i just thought it would be funny to have a meta database in a special agent
	// called fox mulder ... but i guess that we should remove this ...
	Database *meta = ((SpecialAgent *) agent)->getMetaDatabase();
	Database *db   = agent->getDatabase();

	// get some meta information
	IntegerData *num_adds =  (IntegerData* ) meta->getDataItem("number_of_adds");
	IntegerData *num_cells = (IntegerData* ) meta->getDataItem("total_number_of_cells");

	//	IntegerData *num_time_steps_between_outputs = (IntegerData* ) meta->getDataItem("number_between_outputs");
	//	IntegerData *counter = (IntegerData* ) meta->getDataItem("counter");

	// data items needed for resolved pstat5 output
	DoubleData *time_for_output = (DoubleData*) meta->getDataItem("time_to_do_single_output");
	BoolData *b1 = (BoolData*) meta->getDataItem("have_done_single_pstat5_output_for_t_cells");
	IntegerData *num_adds_for_new = (IntegerData*) meta->getDataItem("number_of_adds_for_new");
     
	// one time only output
	if (!b1->getBool() && time_for_output->getDouble() < arg->at(0)) {
	  // accumulate data
	  TVectorData<double> *pstat5_for_new = (TVectorData<double> *) db->getDataItem("pstat5_for_new");
	  pstat5_for_new->at(arg->at(1)-1) = arg->at(8);
	  num_adds_for_new->setInteger(num_adds_for_new->getInteger()+1);
	  if (num_adds_for_new->getInteger() == num_cells->getInteger() ) {
	    // do the output
	    ofstream localout("one_time_only_pstat5_teff.dat");
	    for (int i=0; i<pstat5_for_new->size(); i++)
	      localout << i << " " << pstat5_for_new->at(i) << endl;
	    localout.close();
	    b1->setBool(true);
	  }
	}

	// accumulate the data
	if (num_adds->getInteger() == 0) {
		// fill data
		TVectorData<double> *conc = (TVectorData<double> *) db->getDataItem("total_conc");
		for (int i=2; i<arg->size(); i++)
			conc->at(i-2) = arg->at(i);
	} else {
		TVectorData<double> *conc = (TVectorData<double>*) db->getDataItem("total_conc");
		for (int i=2; i<arg->size(); i++)
			conc->at(i-2) = conc->at(i-2) + arg->at(i);
	}
	num_adds->setInteger(num_adds->getInteger()+1);

	// check whether output has to be generated
	if (num_adds->getInteger() == num_cells->getInteger() ) { //&&
	  //	counter->getInteger() == num_time_steps_between_outputs->getInteger()-1) {
	  //	cerr << "JETZT BIN ICH HIER DIRIN UND SOLLTE ETWAS AUSGEBEN!!!" << endl;
		TVectorData<double> *conc = (TVectorData<double>*) db->getDataItem("total_conc");
		DoubleData *gt = (DoubleData* ) meta->getDataItem("global_time");
		DoubleData *dt = (DoubleData* ) meta->getDataItem("time_step");
		// output time;
		*this->out << setw(12) << setfill(' ') << gt->getDouble(); // *num_time_steps_between_outputs->getInteger();
		// update time
		gt->setDouble(gt->getDouble() + dt->getDouble());
		// output conc vector
		for (int i=0; i<conc->size(); i++)
		  *this->out << setw(12) << setfill(' ') << conc->at(i)/num_cells->getInteger();
		*this->out << endl;
		// reset num_adds
		num_adds->setInteger(0);
		// reset counter
		// counter->setInteger(0);
	} 

}

void SpecialAgentAccumulateDataAction::prepareOutputfile(string name) {
	this->out = new ofstream(name.c_str());
}

SpecialAgentOutputGlobalIL2ConcentrationsAction::SpecialAgentOutputGlobalIL2ConcentrationsAction() :
	Action(TranswellActionIDs::SPECIAL_AGENT_OUTPUT_GLOBAL_CONCENTRATIONS_ID) {
  this->out = NULL;
}

SpecialAgentOutputGlobalIL2ConcentrationsAction::~SpecialAgentOutputGlobalIL2ConcentrationsAction() {
  delete out;
}

void SpecialAgentOutputGlobalIL2ConcentrationsAction::prepareOutputfile(string name) {
	this->out = new ofstream(name.c_str());
}

void SpecialAgentOutputGlobalIL2ConcentrationsAction::fire(Data *d) {
	if (this->agent == NULL)
		throw HiveException("action was not assigned to an agent", "SpecialAgentOutputGlobalIL2ConcentrationsAction::fire(Data *d)");
	if (d->getType().compare("tvectordata_double")!=0)
		throw HiveException("wrong data passed to action", "SpecialAgentOutputGlobalIL2ConcentrationsAction::fire(Data *d)");
	// do the data conversion for the send data
	TVectorData<double> *il2 = (TVectorData<double> *) d;
	// just outputting the send data
	*this->out << setw(12) << setfill(' ') << il2->at(0) << setw(12) << setfill(' ') << il2->at(1)
	           << setw(12) << setfill(' ') << il2->at(2) <<  endl;

}

SpecialAgentOutputIL2ProfileAction::SpecialAgentOutputIL2ProfileAction() :
	Action(TranswellActionIDs::SPECIAL_AGENT_OUTPUT_IL2_PROFILE_ID) { this->out = NULL; }

SpecialAgentOutputIL2ProfileAction::~SpecialAgentOutputIL2ProfileAction() {}

void SpecialAgentOutputIL2ProfileAction::prepareOutputfile(int index) {
  /// ist doch imgrunde unnoetig diese methode hier 
  ostringstream name;
  name << "profile" << index << ".dat";
  this->out = new ofstream(name.str().c_str());
}

void SpecialAgentOutputIL2ProfileAction::fire(Data *d) {
	if (this->agent == NULL)
		throw HiveException("action was not assigned to an agent", "SpecialAgentOutputIL2ProfileAction::fire(Data *)");
	if (d->getType().compare("tvectordata_double") != 0)
		throw HiveException("wrong data passed to action", "SpecialAgentOutputIL2ProfileAction::fire(Data*)");
	// do the data conversion
	TVectorData<double> *dvec = (TVectorData<double> *) d;
	
	//	cerr << "in specialagenoutputil2profileaction: " <<  dvec->size() << " " << dvec->at(0) << endl;
	//	this->prepareOutputfile((int)dvec->at(0));
	ostringstream name;
	name << "profile" << (int)dvec->at(0) << ".dat";
	ofstream *lout = new ofstream(name.str().c_str());
	double x,y,v;
	double stepwidth = 0.1;
	for (int i=1; i<dvec->size();i+=3) {
		x = dvec->at(i)-stepwidth/2.;
		y = dvec->at(i+1)-stepwidth/2;
		v = dvec->at(i+2);
		*lout << x << " " << y << " " << v << endl;
		*lout << x+stepwidth << " " << y << " " << v << endl;
		*lout << x+stepwidth << " " << y+stepwidth << " " << v << endl;
		*lout << x << " " << y+stepwidth << " " << v << endl;
		*lout << x << " " << y << " " << v << endl;
		*lout << endl << endl;
	}
	lout->close();
	delete lout;
}

SpecialAgentAccumulateTregDataAction::SpecialAgentAccumulateTregDataAction() :
	Action(TranswellActionIDs::SPECIAL_AGENT_ACCUMULATE_TREG_DATA_ACTION_ID) {
	out = NULL;
}

SpecialAgentAccumulateTregDataAction::~SpecialAgentAccumulateTregDataAction() {
	delete out;
}

void SpecialAgentAccumulateTregDataAction::fire(Data *d) {
//	cerr << "# inside specialAgentAccumulatedata" << endl;
	if (this->agent == NULL)
		throw HiveException("action was not assigned to an agent", "SpecialAgentAccumulateDataAction::fire(Data *d)");
	if (d->getType().compare("tvectordata_double")!=0)
		throw HiveException("wrong data passed to action", "SpecialAgentAccumulateDataAction::fire(Data *d)");

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
	if (num_adds->getInteger() == num_cells->getInteger() ) {
		TVectorData<double> *conc = (TVectorData<double>*) db->getDataItem("total_conc");
		DoubleData *gt = (DoubleData* ) meta->getDataItem("time_for_treg");
		DoubleData *dt = (DoubleData* ) meta->getDataItem("time_step");
		// output time;
		*this->out << setw(12) << setfill(' ') << gt->getDouble(); // *num_time_steps_between_outputs->getInteger();
		// update time
		gt->setDouble(gt->getDouble() + dt->getDouble());
		// output conc vector
		for (int i=0; i<conc->size(); i++)
			*this->out << setw(12) << setfill(' ') << conc->at(i)/num_cells->getInteger();
		*this->out << endl;
		// reset num_adds
		num_adds->setInteger(0);
	}

}

void SpecialAgentAccumulateTregDataAction::prepareOutputfile(string name) {
	this->out = new ofstream(name.c_str());
}
