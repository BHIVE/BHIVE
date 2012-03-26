#include "worldagentfactoryII.hh"

using namespace ChemoPop;

WorldAgentFactoryII::WorldAgentFactoryII() {
	cerr << "# hello, i am the WorldAgentFactoryII " << endl;

	this->stringswitch["linear"]      = 0;
	this->stringswitch["exponential"] = 1;
	this->stringswitch["pointsource"] = 2;
	this->stringswitch["yuhaissine"]  = 3;
	this->stringswitch["triangle"]  = 4;

	isBlindAgent = false;
	blockWorldOutput = false;
	hasEnvironmentSimulator = false;

	this->generate_cell_position_output = false;
	this->generate_run_tumble_output = false;
	this->generate_traj_before_tumble_output = false;
	this->generateTrajBeforeTumbleTimeOFFSET = 30;
	this->generateTrajBeforeTumbleTimeSTART = 0;
	this->generateTrajBeforeTumbleTimeEND = 1e30;
	this->outputsuffix = "";
	this->output_interval = 1;
}

WorldAgentFactoryII::~WorldAgentFactoryII() { }

void WorldAgentFactoryII::setInput(string in) {
	this->inputname = in;
}

void WorldAgentFactoryII::setNumberCells(int i) {
	this->numbercells = i;
}

void WorldAgentFactoryII::setIsBlindAgentCells(bool isBlindAgent) {
	this->isBlindAgent = isBlindAgent;
}

Agent* WorldAgentFactoryII::createAgent() {
	cerr << "# creating worldagent" << endl;
	// create world agent and get his database
	Agent *world = new Agent();
	Hive::Database *db = world->getDatabase();

	/// this value will be overwritten by the parser if we read cell positions
	IntegerData *numberofcells = new IntegerData("numberofcells", this->numbercells);
	db->addData(numberofcells->getName(), numberofcells);

	// DATA FOR OUTPUT ... this is passed via the command line
	DoubleData *output_interval = new DoubleData("output_interval",this->output_interval);
	db->addData(output_interval->getName(), output_interval);

	// set up agent by using a parser
	// open the relevant inputstream
	ifstream input(this->inputname.c_str());
	// set up parser;  GWA = Garrit's World Agent
	GWAParser gwap;
	cerr << "# starting to invoke GWAParser for parsing the environment" << endl;
	cerr << "# reading from file:    '" << this->inputname <<"'"<< endl;
	gwap.setTumbleTriggeredAverageParameters(this->generate_traj_before_tumble_output,
					this->generateTrajBeforeTumbleTimeOFFSET,
					this->generateTrajBeforeTumbleTimeSTART,
					this->generateTrajBeforeTumbleTimeEND);
	gwap.setInputfile(input);
	gwap.setAgent(world);
	gwap.parse();
	this->equilibration_time = gwap.getEquilibrationTime();
	cerr << "# done parsing input file" << endl;

	// get some information from the database
	// first the cell number such that the registrar can be initialised correctly
	this->numbercells = ((IntegerData*) db->getDataItem("numberofcells"))->getInteger();
	// set the timestep for initialising the simulators
	this->dt = ((DoubleData*) db->getDataItem("dt"))->getDouble();

	// ------------------ Data Structures for the registrar --------------------------------
	// data structure for knowing which cell is stored at which index in the various vectors
	// that the world agent has
	// maps agent_id (key) on to vector index (value)
	MapIntIntData *local_index_map = new MapIntIntData("local_index_map");
	for (int i=0; i<this->numbercells; i++) {
		//assume agent_ids start at 1 (because the world is agent zero)
		local_index_map->insert(i+1,i);
	}
	db->addData(local_index_map->getName(),local_index_map);

	// maps vector_index (key) onto agent_id
	// this is needed for outputing the positions of the cells
	MapIntIntData *index_agentid_map = new MapIntIntData("indexagentidmap");
	for (int i=0; i<this->numbercells; i++) {
		// again, agent_ids = i+1
		index_agentid_map->insert(i,i+1);
	}
	db->addData(index_agentid_map->getName(), index_agentid_map);


	/// now we add the remaining things to the world agent
	/// add simulators to agent
	this->addSimulatorToAgent(world);
	cerr << "# added simulators to the world agent" << endl;

	/// add actions to agents action set
	this->addActionsToAgentsActionSet(world);
	cerr << "# added actions to the world agent" << endl;

	/// add message generators to action set
	this->addMessageGeneratorsToAgent(world);
	cerr << "# added message generators to the world agent" << endl;

	cerr<<"# done creating world"<<endl;
	return world;
}


Agent* WorldAgentFactoryII::createAgent(Action **as) {
	// should throw an exception, not implemented
	throw HiveException("not implemented", "Agent* WorldAgentFactoryII:createAgent(Action **as)");

	Agent *frank = new Agent();
	return frank;
}


Agent* WorldAgentFactoryII::duplicateAgent(Agent *ag) {
	// should throw an exception, not implemented
	throw HiveException("not implemented", "Agent* WorldAgentFactoryII::duplicateAgent(Agent *ag)");

	Agent *frank = new Agent();
	return frank;
}


void WorldAgentFactoryII::finalise() { }


void WorldAgentFactoryII::addSimulatorToAgent(Agent* ag) {
	// add the correct cell movement simulator to the world
	if(this->isBlindAgent) {
		// add blind movement simulator for the world
		if (this->numbercells > 0) {
			cerr << "# adding blind movement simulator to world" << endl;
			BlindMovementSimulatorInWorld * bmsiw = new BlindMovementSimulatorInWorld();
			bmsiw->setWorldAgent(ag);
			ag->addSimulator(bmsiw);
			// add a simulator to model the metabolism consumption
			SimplestConsumptionSimulatorForWorld *scs = new SimplestConsumptionSimulatorForWorld(ag);
			ag->addSimulator(scs);
		}
	} else {
		// add chemotaxis movement simulator to the world
		if (this->numbercells > 0) {
			cerr << "# adding chemotaxis movement simulator to the world" << endl;
			ChemotaxisMovementSimulatorInWorld *cmsiw = new ChemotaxisMovementSimulatorInWorld();
			cmsiw->setGenerateRunTumbleOutput(this->generate_run_tumble_output,"runTumbleTimes"+this->outputsuffix+".dat");
			cmsiw->setGenerateXDirTrajBeforeTumbleOutput(this->generate_traj_before_tumble_output);
			cmsiw->setWorldAgent(ag);
			// add grid to simulator if there is grid environment
			if(((BoolData *)ag->getDatabase()->getDataItem("is_grid_environment"))->getBool()==true) {
				cmsiw->setGrid("environment");
			}
			ag->addSimulator(cmsiw);
		}
		// if there is ought to be an environment simulator in the world, add it
		if (ag->getDatabase()->doesDataItemExist("has_random_drop_simulator")) {
			cerr << "# adding de luetten schieter" << endl;
			DeLuettenSchieterSimulator *dlss = new DeLuettenSchieterSimulator(ag);
			dlss->setEquilibrationTime(((DoubleData* )ag->getDatabase()->getDataItem("eqTime"))->getDouble());
			dlss->setAgent(ag);
			ag->addSimulator(dlss);
			// call testing routine
			// dlss->runTest();
		}
	}
	/// only include the SimplestConsumptionSimulatorForWorld if the flag has been set when parsing the file.
	/// where is it?!

	/// add simulator for diffusion .... this will only be important, if we want
	/// to simulate diffusion on grids ...
	if(((BoolData *)ag->getDatabase()->getDataItem("is_grid_environment"))->getBool()==true) {
		cerr << "# adding diffusion simulator" << endl;
		CrankNicolson *crank = new CrankNicolson();
		crank->setNameOfDataStructureToOperateOn("environment");
		crank->setTimeStep(this->dt);
		crank->setAgent(ag);
		ag->addSimulator(crank);
	}

	// add a celltimekeeper to the agent
	TimeKeeper *ctk = new TimeKeeper(ag,"localworldtime");
	ag->addSimulator(ctk);
}


void WorldAgentFactoryII::addMessageGeneratorsToAgent(Agent *ag) {
	// adding message generators to agent.
	if (this->numbercells > 0) {
		WorldNotifyLigandToCellMessageGenerator *wnltcmg = new WorldNotifyLigandToCellMessageGenerator(ag);
		ag->addMessageGenerator(wnltcmg);
	}

	if(this->isBlindAgent && this->numbercells > 0) {
		WorldNotifyCellAboutConsumedNutrientMessageGenerator *wncacn = new WorldNotifyCellAboutConsumedNutrientMessageGenerator(ag);
		ag->addMessageGenerator(wncacn);
	}

	if (this->numbercells > 0) {
		if (this->generate_cell_position_output) {
			WorldTellSpecialAgentCellPositionsMessageGenerator *wtsacpmg = new WorldTellSpecialAgentCellPositionsMessageGenerator(ag);
			ag->addMessageGenerator(wtsacpmg);
		}

		WorldTellSpecialAgentAnalysedCellPositionsDataMessageGenerator *wtsaacpdm = new WorldTellSpecialAgentAnalysedCellPositionsDataMessageGenerator(ag);
		ag->addMessageGenerator(wtsaacpdm);
	}

	// only output world if requested
	if(!this->blockWorldOutput) {
		// brilliant translation! you should consider joining the united nations as an interpreter
		//	cerr << "# HIER HIER HIER (english translation: not blocking world output in worldagentfactoryII)" << endl;
		WorldTellSpecialAgentAboutGridMessageGenerator *wtsaagmg = new WorldTellSpecialAgentAboutGridMessageGenerator(ag);
		ag->addMessageGenerator(wtsaagmg);
	}
}


void WorldAgentFactoryII::addActionsToAgentsActionSet(Agent *ag) {
	/// we only need to add these actions if we have cells in the environment ...
	if (this->numbercells > 0) {
		UpdateWorldSeenCellsSwimmingStateAction *uwscssa = new UpdateWorldSeenCellsSwimmingStateAction();
		uwscssa->setAgent(ag);
		ag->addAction(uwscssa);
		cerr << "# added UpdateWorldSeenCellsSwimmingStateAction" << endl;

		/// no idea for what we need this action. do we change the speed of the cells
		/// during the simulation?
		UpdateWorldSeenCellSpeedAction *uwscsa = new UpdateWorldSeenCellSpeedAction();
		uwscsa->setAgent(ag);
		ag->addAction(uwscsa);
		cerr << "# added UpdateWorldSeenCellSpeedAction" << endl;
	}

	// only add UpdateWorldBackgroundConcentrationAction, if the environment is not a grid environment
	if (!((BoolData*)ag->getDatabase()->getDataItem("is_grid_environment"))->getBool()) {
		UpdateWorldBackgroundConcentrationAction *uwbca = new UpdateWorldBackgroundConcentrationAction();
		uwbca->setAgent(ag);
		ag->addAction(uwbca);
		cerr << "# added UpdateWorldBackgroundConcentrationAction" << endl;
	}


	/// the actions below only need to be added to the world, if the
	/// world contains blind agents
	if (this->isBlindAgent) {
		UpdateWorldSeenBlindAgentsAnglesAction *uwsbaaa = new UpdateWorldSeenBlindAgentsAnglesAction();
		uwsbaaa->setAgent(ag);
		ag->addAction(uwsbaaa);
		cerr << "# added UpdateWorldSeenBlindAgentsAnglesAction" << endl;

		UpdateWorldDesiredConsumptionAction *uwdca = new UpdateWorldDesiredConsumptionAction();
		uwdca->setAgent(ag);
		ag->addAction(uwdca);
		cerr << "# added UpdateWorldDesiredConsumptionAction" << endl;

		UpdateWorldBlindAgentDeathAction * uwbada = new UpdateWorldBlindAgentDeathAction();
		uwbada->setAgent(ag);
		ag->addAction(uwbada);
		cerr << "# added UpdateWorldBlindAgentDeathAction" << endl;

		UpdateWorldBlindAgentBirthAction * uwbaba = new UpdateWorldBlindAgentBirthAction();
		uwbaba->setAgent(ag);
		ag->addAction(uwbaba);
		cerr << "# added UpdateWorldBlindAgentBirthAction" << endl;
	}
}
