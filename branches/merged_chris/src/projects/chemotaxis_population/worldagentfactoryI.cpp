#include "worldagentfactoryI.hh"



using namespace ChemoPop;

WorldAgentFactoryI::WorldAgentFactoryI() {
	cerr << "# hello, i am the WorldAgentFactoryI " << endl;
	cerr << "# at your service" << endl;

	this->stringswitch["linear"]      = 0;
	this->stringswitch["exponential"] = 1;
	this->stringswitch["pointsource"] = 2;
	this->stringswitch["yuhaissine"]  = 3;
	isBlindAgent = false;
	blockWorldOutput = false;
}

WorldAgentFactoryI::WorldAgentFactoryI(InputSystemReader *isr) {
	cerr << "# nothing" << endl;
	isBlindAgent = false;
}

WorldAgentFactoryI::~WorldAgentFactoryI() { }

void WorldAgentFactoryI::setInput(string in) {
	this->inputname = in;
}

void WorldAgentFactoryI::setNumberCells(int i) {
	this->numbercells = i;
}

void WorldAgentFactoryI::setIsBlindAgentCells(bool isBlindAgent) {
	this->isBlindAgent = isBlindAgent;
}

Agent* WorldAgentFactoryI::createAgent() {
	// open up an inputstream
//	cerr << this->inputname << endl;
	ifstream input(this->inputname.c_str());

	// create world agent and get his database
	Agent *world = new Agent();
	Hive::Database *db = world->getDatabase();

	// add the time counter to the world
	DoubleData *localworldtime = new DoubleData("localworldtime",0.0);
	db->addData(localworldtime->getName(),localworldtime);

	DoubleData *eqTime         = new DoubleData("eqTime", 0);
	db->addData(eqTime->getName(), eqTime);

	DoubleData *dt = new DoubleData("dt", 0);
	db->addData(dt);

	IntegerData *numberofcells = new IntegerData("numberofcells", this->numbercells);
	db->addData(numberofcells->getName(), numberofcells);

	// data structure for knowing which cell is stored at which index in the various vectors
	// that the world agent has
	// maps agent_id (key) on to vector index (value)
	MapIntIntData *local_index_map = new MapIntIntData("local_index_map");
	for (int i=0; i<this->numbercells; i++) {
		//assume agent_ids start at 1 (because the world is agent zero)
		local_index_map->insert(i+1,i);

		//cout<<"adding to local index map: ["<<i+1<<"]: => "<<i<<endl;
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

	// add data structure to the world in which it can store the cell positions
	TVectorData<TVectorData<double>* > *cell_positions = new TVectorData<TVectorData<double>* > ("cellpositions","tvectordata_double_matrix");
	cell_positions->reserveSize(this->numbercells);

	// add data_structure for direction and up vector of the individual cells
	TVectorData<TVectorData<double>* > *cell_dir_vecs = new TVectorData<TVectorData<double>* > ("celldirvecs", "tvectordata_doublematrix");
	cell_dir_vecs->reserveSize(this->numbercells);

	TVectorData<TVectorData<double>* > *cell_up_vecs  = new TVectorData<TVectorData<double>* > ("cellupvecs", "tvectordata_doublematrix");
	cell_up_vecs->reserveSize(this->numbercells);

	for (int i=0; i<this->numbercells;i++) {
		(*cell_positions)[i] = new TVectorData<double> ("position", "tvectordata_double");
		(*cell_positions)[i]->reserveSize(3);
		(*cell_dir_vecs)[i]  = new TVectorData<double> ("direction", "tvectordata_double");
		(*cell_dir_vecs)[i]->reserveSize(3);
		/// this needs to be a unit-vector.
		cell_dir_vecs->at(i)->at(0) = 1.; cell_dir_vecs->at(i)->at(1) = 0.; cell_dir_vecs->at(i)->at(2) = 0;
		(*cell_up_vecs)[i]   = new TVectorData<double> ("up", "tvectordata_double");
		(*cell_up_vecs)[i]->reserveSize(3);
		cell_up_vecs->at(i)->at(0) = 0; cell_up_vecs->at(i)->at(1) = 1; cell_up_vecs->at(i)->at(2) = 0;
	}
	db->addData(cell_positions->getName(), cell_positions);
	db->addData(cell_dir_vecs->getName(), cell_dir_vecs);
	db->addData(cell_up_vecs->getName(), cell_up_vecs);

	// data structure storing how much a cell wishes to consume
	// we will also use it to store the actual concentration that gets consumed by the cells
	TVectorData<double> *desired_cell_consumption = new TVectorData<double>("desired_cell_consumption", "tvectordata_double");
	desired_cell_consumption->reserveSize(this->numbercells);
	db->addData(desired_cell_consumption->getName(), desired_cell_consumption);

	// time step for movement of the cells
	DoubleData *movement_dt = new DoubleData("movement_dt",1.);
	db->addData(movement_dt->getName(), movement_dt);

	/// speed of cells (is that the same for all the cells?)
	//	DoubleData *cellspeed = new DoubleData("cellspeed",0.1);
	//	db->addData(cellspeed->getName(), cellspeed);

	TVectorData<double> *cellspeeds = new TVectorData<double>("cellspeeds", "tvector_double");
	cellspeeds->reserveSize(this->numbercells);
	for (int i=0; i<this->numbercells; i++)
		cellspeeds->at(i) = 20.0;
	db->addData(cellspeeds->getName(), cellspeeds);

	TVectorData<int> *cell_wants_to_move = new TVectorData<int>("cell_wants_to_move", "tvector_bool");
	cell_wants_to_move->reserveSize(this->numbercells);
	for (int i=0; i<this->numbercells; i++)
		cell_wants_to_move->at(i) = (int)true;
	db->addData(cell_wants_to_move->getName(), cell_wants_to_move);

	DoubleData *output_interval = new DoubleData("output_interval",this->output_interval);
	db->addData(output_interval->getName(), output_interval);


	BoolData *isGridEnv = new BoolData("is_grid_environment",false);
	db->addData(isGridEnv);



	if(!this->isBlindAgent) {
		cerr << "# cells are of type: ECOLI" << endl;
		// add data structures to the world that are unique to the E.coli model

		// add data structure to the world in which it can store the swimming states of all the cells
		TVectorData<int> *swimming_states = new TVectorData<int>("swimmingstates", "tvectordata_int");
		swimming_states->reserveSize(this->numbercells);
		db->addData(swimming_states->getName(), swimming_states);

		// add data structure to the world in whihc it stores the old swimming states of all the cells
		TVectorData<int> *last_swimming_states = new TVectorData<int>("lastswimmingstates", "tvectordata_int");
		last_swimming_states->reserveSize(this->numbercells);
		db->addData(last_swimming_states->getName(), last_swimming_states);

		// rotational diffusion constant of the cells
		DoubleData *rotdiffconst = new DoubleData("rotational_diffusion_constant",0.062);
		db->addData(rotdiffconst->getName(), rotdiffconst);
	} else {
		cerr << "# cells are of type: BLIND" << endl;

		// add data object needed by the blind searcher, which is the
		// next angle to use during turns.  Default to zero and must be
		// overridden by the agent
		TVectorData<double> *nextTurnAngles = new TVectorData<double>("cellnextturnangle", "tvector_double");
		nextTurnAngles->reserveSize(this->numbercells);
		for (int i=0; i<this->numbercells; i++)
			nextTurnAngles->at(i) = 0;
		db->addData(nextTurnAngles->getName(), nextTurnAngles);
	}



	// do the parsing of the system file. we can extend this later on if more information is needed.

	if(!input.is_open()) {
		cerr<<"did not open world file. quitting.";
		exit(1);
	}

	input.seekg(0,ios_base::beg);
	string line = "";
	istringstream iss;
	string environ_type;
	string cellpos_init_type;
	CellPositionInitialiser cpi;
	while (getline(input,line)) {
		if (line == "TYPE") {
			while (getline(input,line) && line != "END") {
				if (line.substr(0,1)!="#" && line != "") {
					iss.clear();
					iss.str(line);
					iss >> environ_type;
					if (environ_type == "linear") {
						cerr << "# creating linear environment" << endl;
						OneDLinearMathFunctionData *environ1 = new OneDLinearMathFunctionData("environment");
						double s, yi;
						iss >> s; iss >> yi;
						environ1->setSlope(s);
						environ1->setYIntercept(yi);
						db->addData(environ1->getName(), environ1);
					} else if (environ_type == "exponential") {
						cerr << "# creating exponential environment" << endl;
						OneDExponentialMathFunctionData *environ2 = new OneDExponentialMathFunctionData("environment");
						double o, p;
						iss >> o, iss >> p;
						environ2->setOffset(o);
						environ2->setParameter(p);
						db->addData(environ2->getName(), environ2);
					} else if (environ_type == "pointsource") {
						cerr << "# creating pointsource environment" << endl;
						CarlFriedrichMathFunctionData *environ3 = new CarlFriedrichMathFunctionData("environment");
						double D, n, x, y, z;
						iss >> D; iss >> n; iss >> x; iss >> y; iss >> z;
						environ3->setDiff(D);
						environ3->setInitialNumber(n);
						environ3->setPosition(x,y,z);
						db->addData(environ3->getName(), environ3);
					} else if (environ_type == "gauss") {
						cerr << "# creating a gauss curve" << endl;
						GaussGlockeMathFunctionData *environ3b = new GaussGlockeMathFunctionData("environment");
						double a, s, x, y, z;
						iss >> a; iss >> s; iss >> x; iss >> y; iss >> z;
						environ3b->setAmplitude(a);
						environ3b->setSigma(s);
						environ3b->setPosition(x,y,z);
						db->addData(environ3b->getName(), environ3b);
					} else if (environ_type == "doublegauss") {
						cerr << "#creating double gauss curve" << endl;
						DoubleGaussFunctionData *environ3c = new DoubleGaussFunctionData("environment");
						double a0, a1, s0, s1, x0, y0, z0, x1, y1, z1;
						iss >> a0; iss >> s0; iss >> x0; iss >> y0; iss >> z0;
						iss >> a1; iss >> s1; iss >> x1; iss >> y1; iss >> z1;
						environ3c->setAmplitude0(a0);
						environ3c->setSigma0(s0);
						environ3c->setPosition0(x0,y0,z0);
						environ3c->setAmplitude1(a1);
						environ3c->setSigma1(s1);
						environ3c->setPosition1(x1,y1,z1);
						db->addData(environ3c->getName(), environ3c);
					} else if (environ_type == "yuhaissine") {
						cerr << "# creating yuhai's sine-linear environment" << endl;
						YuhaisSineFunctionData *environ4 = new YuhaisSineFunctionData("environment");
						double h, o, s;
						iss >> h; iss >> o; iss >> s;
						environ4->setHalfConcentration(h);
						environ4->setOmega(o);
						environ4->setSlope(s);
						db->addData(environ4->getName(), environ4);
					} else if (environ_type == "step") {
						cerr << "# creating step environment" << endl;
						StepFunction *environ5 = new StepFunction("environment");

						// hard coded steps, but this can easily be read in from file if need be
						vector <double> steps;
						vector <int> times;


						steps.push_back(0.0001); // in Molar, so this is 10uM
						times.push_back(150);

						steps.push_back(0.001); // inMolar, this is 1mM
						times.push_back(200);

						steps.push_back(0); // back to zero
						times.push_back(250);

						environ5->setSteps(steps,times);
						db->addData(environ5->getName(), environ5);
					} else if (environ_type == "cube_grid") {

						DoubleData* totalNutrient = new DoubleData("total_nutrient",0);
						db->addData(totalNutrient);


						// tell the cell position initialiser that it is dealing with a cubic grid
						cpi.setIsCubeGrid(true);
						// declare a cubegeometry constructor.
						CubeGeometryConstructor cgc;
						double l, r;
						iss >> l; iss >> r;
						cgc.setLength(l);
						cgc.setResolution(r);
						Grid *g = cgc.returnGrid("environment");
						g->isCubeGrid(true);

						// we have to set the number of species to one
						// this is not very nice and should be move to the cubegeometry-constructor
						for (int i=0; i<g->getNumberOfVoxels(); i++)
							g->getVoxel(i)->setNumberSpecies(1);
						g->addSpecies("nutrient",0.0);
						db->addData(g->getName(),g);

						// we have to add a datastructure for storing in which voxels the cells are situated.
						TVectorData<int> *cell_voxel_positions = new TVectorData<int>("cell_voxel_positions","tvector_int");
						cell_voxel_positions->reserveSize(this->numbercells);
						db->addData(cell_voxel_positions->getName(),cell_voxel_positions);

						// data structure which tells how many cells there are in each voxel
						TVectorData<int> *number_cells_per_voxel = new TVectorData<int>("number_cells_per_voxel", "tvector_int");
						number_cells_per_voxel->reserveSize(g->getNumberOfVoxels());
						db->addData(number_cells_per_voxel->getName(), number_cells_per_voxel);

						isGridEnv->setBool(true);

						string initType = "";
						iss >> initType;
						Util::trim(initType);
						if(initType.compare("UNIFORM")==0) {

							double conc = 0;
							iss >> conc;
							CubeConstantInitialiser * cci = new CubeConstantInitialiser();
							cci->setGrid(g);
							cci->setValue(conc);
							cci->initialise();
							delete cci;

						} else if (initType.compare("RANDOM")==0) {
							 int num = 0;
							 double min = 0, max = 0;
							 iss >> num;
							 iss >> min;
							 iss >> max;
							 CubeRandomInitialiser *cri = new CubeRandomInitialiser();
							 cri->setGrid(g);
							 cri->setNumberOfPoints(num);
							 cri->setNutrientRange(min,max);
							 cri->initialise();
							 delete cri;
						} else {
							throw(HiveException("Invalid initialization scheme specified for cube_grid","WorldAgentFactoryI::createAgent()"));
						}

						// be sure to save the cubes side length and step size
						DoubleData * cubeSideLength = new DoubleData("cube_side_length",l);
						db->addData(cubeSideLength);

						DoubleData * cubeStepSize = new DoubleData("cube_step_size",r);
						db->addData(cubeStepSize);


					} else {
						cerr << "# do not understand the input file ... leaving ... " << endl;
						cerr << line << endl;
						exit(1);
					}
				}
			}
		}

		if (line == "NUM_STEPS") {
			while (getline(input,line) && line != "END") {
				if (line.substr(0,1)!="#" && line != "") {
					iss.clear();
					iss.str(line);
					int ns; double ti;
					iss >> ns;
					this->num_steps = ns;
					iss >> ti;
					this->dt = ti;
					movement_dt->setDouble(ti);
				}
			}
		}




		/// here we initialise the positions of the cell positions
		if (line == "CELLPOSITIONS") {
			while (getline(input,line) && line != "END") {
				if (line.substr(0,1)!= "#" && line != "") {
					iss.clear();
					iss.str(line);
					iss >> cellpos_init_type;
					if (cellpos_init_type == "all_same_position") {
						cerr << "# initialising cells all at the same position" << endl;
						double x; double y; double z;
						iss >> x; iss >> y; iss >> z;
						cpi.initAllInSamePosition(world,x,y,z);
					}
					else if (cellpos_init_type == "around_position") {
						cerr << "# initialising cell positions around position" << endl;
						double x, y, z, r;
						iss >> x; iss >> y; iss >> z; iss >> r;
						cpi.initRandomlyAroundPosition(world,x,y,z,r);
					}
					else if (cellpos_init_type == "random_in_cube") {
						cerr << "# initialising cell positions randomly in cube" << endl;
						cpi.initRandomlyInCube(world);
					}
					else if (cellpos_init_type == "from_file") {
						cerr << "# initialising from file" << endl;
						string filename;
						iss >> filename;
						ifstream cellinput(filename.c_str());
						cpi.initFromFile(world,cellinput);
						cellinput.close();
					}
					else {
						cerr << "# do not understand the option ... leaving ... " << endl;
					}
				}
			}
		}
	}


	/// now we add the remaining things to the world agent

	/// add simulators to agent
	this->addSimulatorToAgent(world);

	/// add actions to agents action set
	this->addActionsToAgentsActionSet(world);

	/// add message generators to action set
	this->addMessageGeneratorsToAgent(world);


	cerr<<"# done creating world"<<endl;

	return world;
}

Agent* WorldAgentFactoryI::createAgent(Action **as) {
	Agent *frank = new Agent();

	return frank;
}

Agent* WorldAgentFactoryI::duplicateAgent(Agent *ag) {
	Agent *frank = new Agent();
	return frank;
}

void WorldAgentFactoryI::finalise() { }

void WorldAgentFactoryI::addSimulatorToAgent(Agent* ag) {

	// add the correct cell movement simulator to the world
	if(this->isBlindAgent) {

		BlindMovementSimulatorInWorld * bmsiw = new BlindMovementSimulatorInWorld();
		bmsiw->setWorldAgent(ag);
		ag->addSimulator(bmsiw);

	} else {

		ChemotaxisMovementSimulatorInWorld *cmsiw = new ChemotaxisMovementSimulatorInWorld();
		cmsiw->setWorldAgent(ag);
		ag->addSimulator(cmsiw);
	}

	// add a simulator to model the metabolism consumption
	SimplestConsumptionSimulatorForWorld *scs = new SimplestConsumptionSimulatorForWorld(ag);
	ag->addSimulator(scs);

	// add a celltimekeeper to the agent
	TimeKeeper *ctk = new TimeKeeper(ag,"localworldtime");
	ag->addSimulator(ctk);

}


void WorldAgentFactoryI::addMessageGeneratorsToAgent(Agent *ag) {
	// adding message generators to agent.
	WorldNotifyLigandToCellMessageGenerator *wnltcmg = new WorldNotifyLigandToCellMessageGenerator(ag);
	ag->addMessageGenerator(wnltcmg);

	WorldNotifyCellAboutConsumedNutrientMessageGenerator *wncacn = new WorldNotifyCellAboutConsumedNutrientMessageGenerator(ag);
	ag->addMessageGenerator(wncacn);

	WorldTellSpecialAgentCellPositionsMessageGenerator *wtsacpmg = new WorldTellSpecialAgentCellPositionsMessageGenerator(ag);
	wtsacpmg->isMessageForSpecialAgent();
	ag->addMessageGenerator(wtsacpmg);

	// only output world if requested
	if(!this->blockWorldOutput) {
		WorldTellSpecialAgentAboutGridMessageGenerator *wtsaagmg = new WorldTellSpecialAgentAboutGridMessageGenerator(ag);
		wtsaagmg->isMessageForSpecialAgent();
		ag->addMessageGenerator(wtsaagmg);
	}


}



void WorldAgentFactoryI::addActionsToAgentsActionSet(Agent *ag) {

	// adding actions to agents
	UpdateWorldSeenCellsSwimmingStateAction *uwscssa = new UpdateWorldSeenCellsSwimmingStateAction();
	uwscssa->setAgent(ag);
	ag->addAction(uwscssa);

	UpdateWorldSeenCellSpeedAction *uwscsa = new UpdateWorldSeenCellSpeedAction();
	uwscsa->setAgent(ag);
	ag->addAction(uwscsa);

	UpdateWorldBackgroundConcentrationAction *uwbca = new UpdateWorldBackgroundConcentrationAction();
	uwbca->setAgent(ag);
	ag->addAction(uwbca);

	UpdateWorldSeenBlindAgentsAnglesAction *uwsbaaa = new UpdateWorldSeenBlindAgentsAnglesAction();
	uwsbaaa->setAgent(ag);
	ag->addAction(uwsbaaa);

	UpdateWorldDesiredConsumptionAction *uwdca = new UpdateWorldDesiredConsumptionAction();
	uwdca->setAgent(ag);
	ag->addAction(uwdca);

	UpdateWorldBlindAgentDeathAction * uwbada = new UpdateWorldBlindAgentDeathAction();
	uwbada->setAgent(ag);
	ag->addAction(uwbada);

	UpdateWorldBlindAgentBirthAction * uwbaba = new UpdateWorldBlindAgentBirthAction();
	uwbaba->setAgent(ag);
	ag->addAction(uwbaba);


}
