#include "gwaparser.hh"

using namespace std;
using namespace Hive;

GWAParser::GWAParser(){}

GWAParser::~GWAParser(){}


void GWAParser::doIt() {
	// get the database from the system
	this->db = ag->getDatabase();

	// go to the beginning of the file
	input->seekg(0,ios_base::beg);
	// local variables for parsing
	string line = "";
	istringstream iss;
	string keyword;

	// do the parsing of the input file
	while (getline(*input, line)) {
		Util::trim(line);
		if (line.substr(0,1) != "#" && line != "") {
		//	cerr << line << endl;
			iss.clear();
			iss.str(line);
			iss >> keyword;
			if (keyword == "ENVIRONMENTTYPE") { // read information about the environment
				cerr << "# parsing environment" << endl;
				this->parseEnvironment();
				cerr << "# done parsing environment" << endl;
			}
			if (keyword == "CELL_POSITIONS") {   // initialise the positions of the cells
				cerr << "# parsing cell positions" << endl;
				this->parseCellPositions();
			}
			if (keyword == "NUM_STEPS") {
				cerr << "# parsing number of steps" << endl;
				this->parseTimeSteps();
			}
			if (keyword == "FLAGS") {
				cerr << "# parsing flags" << endl;
				this->parseFlags();
			}
		}
	}
}


void GWAParser::parseEnvironment() {
	/// try to add flag for grid environment
	if (!db->doesDataItemExist("is_grid_environment")) {
		BoolData *b = new BoolData("is_grid_environment", false);
		db->addData(b);
	}
	// locale variables for parsing
	string line = "";
	istringstream iss;
	string entype;
	// get the next line
	while (getline(*input, line) && line != "END") {
		Util::trim(line);
		if (line.substr(0,1) != "#" && line != "") {
			iss.clear();
			iss.str(line);
			iss >> entype;
			// check what environment need to be parsed and call the adequate method
			if (entype == "MATH")
				this->parseMathEnvironment();
			else if (entype == "GRID")
				this->parseGridEnvironment();
			else if (entype == "SIMULATOR")
				this->parseEnvironmentSimulator();
			else
				throw HiveException("Do not understand key word from line: '"+line+"'", "GWAParser::parseEnvironment");
		}
	}
}

void GWAParser::parseFlags() {
	// some local variables

	cerr<<"# in GWAParser: parsing flags"<<endl;
	string line = "";
	istringstream iss;
	string flagtype;
	string flagvalue;
	// get the next line
	while (getline(*input, line) && line != "END") {
		Util::trim(line);
		if (line.substr(0,1) != "#" && line != "") {
			iss.clear();
			iss.str(line);
			iss >> flagtype;
			iss >> flagvalue;
			// cells will have a metabolism model
			if (flagtype == "CELL_HAS_METABOLISM") {
				// right now the only understood value is simple
				if (flagvalue == "simple" || flagvalue == "SIMPLE" || flagvalue == "Simple") {
					this->addSimpleMetabolismStructures();
				} else {
					throw HiveException("# do not understand the metabolism model to be used","GWAParser::parseFlags()");
				}
			} else if (flagtype == "CELLTYPE") { // parsing the celltype
				cerr<<"# in GWAParser: parsing CELLTYPE as: "<<flagvalue<<endl;
				this->setCellType(flagvalue);
			} else {
				throw HiveException("# do not understand the given flag: '"+line+"'", "GWAParser::parseFlags()");
			}
		}
	}
}


void GWAParser::setCellType(string value) {
	if (value == "ecoli" || value == "Ecoli" || value == "ECOLI") {
		/// add data structures to agent that are relevant for ecoli chemotaxis simulations
		if (!db->doesDataItemExist("swimmingstates")) {
			TVectorData<int> *swimming_states = new TVectorData<int>("swimmingstates", "tvectordata_int");
			swimming_states->reserveSize(0);
			db->addData(swimming_states->getName(), swimming_states);
		}
		// add data structure to the world in whihc it stores the old swimming states of all the cells
		if (!db->doesDataItemExist("lastswimmingstates")) {
			TVectorData<int> *last_swimming_states = new TVectorData<int>("lastswimmingstates", "tvectordata_int");
			last_swimming_states->reserveSize(0);
			db->addData(last_swimming_states->getName(), last_swimming_states);
		}
		// try to add rotational diffusion constant to world
		if (!db->doesDataItemExist("rotational_diffusion_constant")) {
			// rotational diffusion constant of the cells
			DoubleData *rotdiffconst = new DoubleData("rotational_diffusion_constant",0.062);
			db->addData(rotdiffconst->getName(), rotdiffconst);
		}


		// flag indicating that agents are ecoli
		if (!db->doesDataItemExist("is_ecoli")) {
			BoolData *b = new BoolData("is_ecoli",true);
			db->addData(b);
		} else
			((BoolData* ) db->getDataItem("is_ecoli"))->setBool(true);
	} else if (value == "blindagent" || value == "Blindagent" || value == "BlindAgent" || value == "BLINDAGENT") {
		/// add data structures to world agent that are relevant for blindagent simulations
		/// datastructure cellnextturnangle needs to be added
		if (!db->doesDataItemExist("cellnextturnangle")) {
			TVectorData<double> *nextTurnAngles = new TVectorData<double>("cellnextturnangle", "tvector_double");
			nextTurnAngles->reserveSize(0);
			db->addData(nextTurnAngles->getName(), nextTurnAngles);
		}
		/// flag indicating whether agents are blind agents
		if (!db->doesDataItemExist("is_blind_agent")) {
			BoolData *b = new BoolData("is_blind_agent", true);
			db->addData(b);
		} else
			((BoolData *) db->getDataItem("is_blind_agent"))->setBool(true);
	}
}


void GWAParser::addSimpleMetabolismStructures() {
	// data structure for storing how much a cell wishes to consume
	// we will also use it to store the actual concentration that gets consumed by the cells
	if (!db->doesDataItemExist("desired_cell_consumption")) {
		TVectorData<double> *desired_cell_consumption = new TVectorData<double>("desired_cell_consumption", "tvectordata_double");
		desired_cell_consumption->reserveSize(0);
		db->addData(desired_cell_consumption->getName(), desired_cell_consumption);
	}
	if (!db->doesDataItemExist("cells_have_simple_metabolism")) {
		BoolData *b = new BoolData("cells_have_simple_metabolism", true);
		db->addData(b);
	} else
		((BoolData *) db->getDataItem("cells_have_simple_metabolism"))->setBool(false);
}


void GWAParser::parseMathEnvironment() {
	// locale variables for parsing
	string line = "";
	istringstream iss;
	string environ_type;
	// the system will contain math environments, hence we will need an environments vector
	TVectorData<MathFunctionData* > *environments = new TVectorData<MathFunctionData* >("environments", "tvector_mathfunctiondata");
	while (getline(*input, line)) {
		Util::trim(line);
		if(line == "END") { break; }
		if (line.substr(0,1) != "#" && line != "") {
			// push the line in the input string stream
			iss.clear();
			iss.str(line);
			iss >> environ_type;
			if (environ_type == "linear") {
				double slope, yintercept;
				iss >> slope; iss >> yintercept;
				OneDLinearMathFunctionData *environ1 = new OneDLinearMathFunctionData("environment");
				environ1->setSlope(slope);
				environ1->setYIntercept(yintercept);
				environments->addElementToEnd(environ1);
			} else if (environ_type == "exponential") {
				double offset, parameter;
				iss >> offset; iss >> parameter;
				OneDExponentialMathFunctionData *environ2 = new OneDExponentialMathFunctionData("environment");
				environ2->setParameter(parameter);
				environ2->setOffset(offset);
				environments->addElementToEnd(environ2);
			} else if (environ_type == "step") {
				StepResponseMathFunction *environStep = new StepResponseMathFunction("environment");
				environStep->initialize(line);
				environments->addElementToEnd(environStep);
			} else if (environ_type == "pointsource") {
				double D, n, x, y, z;
				iss >> D; iss >> n; iss >> x; iss >> y; iss >> z;
				CarlFriedrichMathFunctionData *environ3 = new CarlFriedrichMathFunctionData("environment");
				environ3->setDiff(D);
				environ3->setInitialNumber(n);
				environ3->setPosition(x,y,z);
				environ3->setT0(0);
				environments->addElementToEnd(environ3);
			} else if (environ_type == "gauss") {
				double a, s, x, y, z;
				iss >> a; iss >> s; iss >> x; iss >> y; iss >> z;
				GaussGlockeMathFunctionData *environ4 = new GaussGlockeMathFunctionData("environment");
				environ4->setAmplitude(a);
				environ4->setSigma(s);
				environ4->setPosition(x,y,z);
				environments->addElementToEnd(environ4);
			} else if (environ_type == "double_gauss") { // note that this environment type by now is redundant
				double a0, a1, s0, s1, x0, x1, y0, y1, z0, z1;
				// read information about the two peaks
				iss >> a0; iss >> s0; iss >> x0; iss >> y0; iss >> z0;
				iss >> a1; iss >> s1; iss >> x1; iss >> y1; iss >> z1;
				DoubleGaussFunctionData *environ5 = new DoubleGaussFunctionData("environment");
				environ5->setAmplitude0(a0);
				environ5->setSigma0(s0);
				environ5->setPosition0(x0,y0,z0);
				environ5->setAmplitude1(a1);
				environ5->setSigma1(s1);
				environ5->setPosition1(x1,y1,z1);
				environments->addElementToEnd(environ5);
			} else if(environ_type == "yuhaissine") {
				double h, o, s;
				iss >> h; iss >> o; iss >> s;
				YuhaisSineFunctionData *environ6 = new YuhaisSineFunctionData("environment");
				environ6->setHalfConcentration(h);
				environ6->setOmega(o);
				environ6->setSlope(s);
				environments->addElementToEnd(environ6);
			} else
				throw HiveException("Do not under math environment to be parsed, which was specified as: '"+environ_type+"'", "GWAParser::parseMathEnvironment");
		}
	}
	// add the environments vector to the database
	db->addData(environments);

	// set all the start times to be zero (we have to do this because the drop simulator
	// uses the same data structure, and sets the specific time of each drop here.
	TVectorData<double> *birth_times_of_environments =
							new TVectorData<double> ("birthtimes_of_environments", "tvector_double");
	birth_times_of_environments->reserveSize(environments->size());
	for(int k=0; k<environments->size(); k++)
		birth_times_of_environments->at(k) = 0.;
	db->addData(birth_times_of_environments);

}

void GWAParser::parseGridEnvironment() {
	string line = "";
	istringstream iss;
	string type;
	// set the grid flag in the world agent
	((BoolData*) db->getDataItem("is_grid_environment"))->setBool(true);
	// parse the input
	while (getline(*input,line) && line != "END") {
		Util::trim(line);
		if (line.substr(0,1) != "#" && line != "") {
			iss.clear();
			iss.str(line);
			iss >> type;
			// build a cube grid by employing the cube grid constructor.
			// note ...
			if (type == "CUBE") {
				ChemoPop::CubeGeometryConstructor cgc;
				// read sidelenght and resolution of grid to be constructed.
				double l, r;
				iss >> l; iss >> r;
				// for the beginning we assume that there only exists a
				// single species in the system of which the diffusion coefficient
				// is read next.
				double D;
				iss >> D;
				vector<string> species_names(1); species_names[0] = "nutrient";
				vector<double> species_diffs(1); species_diffs[0] = D;
				cgc.setSpecies(1, species_names, species_diffs);
				cgc.setLength(l);
				cgc.setResolution(r);
				Grid *g = cgc.returnGrid("environment");
				g->setIsCubeGrid(true);
				db->addData(g);
			} else if (type == "MICROFLUIDIC") { // a microfluidic grid is to be read from a file.
				// read the resolution of the grid and the filename from the file.
				double r; string filename;
				iss >> r; iss >> filename;
				// remember the filename of the geometry. we might need it for reading
				// in the positions of the cells
				this->name_geometry_input_file_fmf = filename;
				// read the filename of the initial concentrations
				string filename2;
				iss >> filename2;
				// we assume again that there only exists a single species. we read its diffusion
				// coefficient from the input file
				double D;
				iss >> D;
				// use the GridReaderForMicroFluidic to set up the environment
				GridReaderForMicroFluidic grfmf;
				grfmf.setResolution(r);
				grfmf.setInputFile(filename);
				grfmf.addSpecies("nutrient",D);
				Grid *g = grfmf.returnGrid();
				grfmf.readInitialConcentrations(filename2);
	//			g->outputGridForValidation();
				db->addData("environment",g);
				// set up a quad-tree
				QuadTreeForMicroFluidic *quad = new QuadTreeForMicroFluidic();
				quad->constructTree(g);
				// add quad tree to the world agent.
				db->addData(quad);
				// tell the world agent that it is an microfluidic simulation
				BoolData *b = new BoolData("microfluidic_flag",true);
				db->addData(b);
			} else
				throw HiveException("Do not understand grid environment to be parsed",
						            "GWAParser::parseGridEnvironment");
		}
	}
	/// done with the parsing part. now we need to add two more data structures
	if (!db->doesDataItemExist("cell_voxel_positions")) {
		TVectorData<int> *cell_voxel_positions = new TVectorData<int> ("cell_voxel_positions", "tvector_int");
		cell_voxel_positions->reserveSize(0);
		db->addData(cell_voxel_positions);
	}
	if (!db->doesDataItemExist("number_cells_per_voxel")) {
		TVectorData<int> *number_cells_per_voxel = new TVectorData<int>("number_cells_per_voxel", "tvector_int");
		number_cells_per_voxel->reserveSize(0);
		db->addData(number_cells_per_voxel);
	}
}

void GWAParser::parseEnvironmentSimulator() {
	// local variables
	string line = "";
	istringstream iss;
	string type;
	while (getline(*input,line)) {
		Util::trim(line);
		if(line == "END") break;
		if (line.substr(0,1) != "#" && line != "") {
			iss.clear();
			iss.str(line);
			iss >> type;
			if (type == "RANDOM_DROP_SIMULATOR") {
				// begin by telling the database that there will be an environment simulator
				// of the random drop type in the system
				BoolData *b = new BoolData("has_random_drop_simulator", true);
				db->addData(b);
				// add relevant data items to the world
				TVectorData<double> *lastposition = new TVectorData<double> ("last_pointsource_position",
																			 "tvector_double");
				lastposition->reserveSize(3);
				lastposition->at(0) = 0;
				lastposition->at(1) = 0;
				lastposition->at(2) = 0;
				db->addData(lastposition);
				// add environments vector
				TVectorData<MathFunctionData* > *environments =
						new TVectorData<MathFunctionData* > ("environments", "tvector_mathfunctiondata");
				db->addData(environments);
				// add birthtimes vector
				TVectorData<double> *birth_times_of_environments =
						new TVectorData<double> ("birthtimes_of_environments", "tvector_double");
			//	birth_times_of_environments->reserveSize(1);
			//	birth_times_of_environments->at(0) = 0.;
				db->addData(birth_times_of_environments);
				// time of next drop
				DoubleData *tone = new DoubleData("environment_time_of_next_drop", 0);
				db->addData(tone);
				/// next we
				double tp, sp, mi, sigma, diff;
				string boolval;
				iss >> tp; iss >> sp; iss >> mi; iss >> sigma; iss >> diff; iss >> boolval;
				DoubleData *d1 = new DoubleData("environment_time_parameter",tp);
				DoubleData *d2 = new DoubleData("environment_distance_parameter", sp);
				DoubleData *d3 = new DoubleData("mean_of_pointsource_initial_distribution", mi);
				DoubleData *d4 = new DoubleData("sigma_of_pointsource_initial_distribution", sigma);
				DoubleData *d5 = new DoubleData("diffusions_coeff_ligand", diff);
				BoolData   *b6 = new BoolData("switch_for_new_drops", false);
				if (boolval == "True" || boolval == "TRUE" || boolval == "true")
					b6->setBool(true);
				else if (boolval == "False" || boolval == "FALSE" || boolval == "false")
					b6->setBool(false);
				db->addData(d1);
				db->addData(d2);
				db->addData(d3);
				db->addData(d4);
				db->addData(d5);
				db->addData(b6);
			} else
				throw HiveException("Do not understand environment simulator",
						            "GWAParser::parseEnvironmentSimulator");
		}
	}

}

void GWAParser::parseCellPositions() {
	// some local variables
	string line = "";
	istringstream iss;
	string type;
	ChemoPop::CellPositionInitialiser cpi;

	//  begin by adding data item for movement of cells to the world that are needed irrespective of celltype
	if (!db->doesDataItemExist("cellpositions")) {
		TVectorData<TVectorData<double>* > *cell_positions = new TVectorData<TVectorData<double>* > ("cellpositions","tvectordata_doublematrix");
		cell_positions->reserveSize(0);
		db->addData(cell_positions);
	}
	// add data_structure for direction and up vector of the individual cells
	if (!db->doesDataItemExist("celldirvecs")) {
		TVectorData<TVectorData<double>* > *cell_dir_vecs = new TVectorData<TVectorData<double>* > ("celldirvecs", "tvectordata_doublematrix");
		cell_dir_vecs->reserveSize(0);
		db->addData(cell_dir_vecs->getName(), cell_dir_vecs);
	}
	if (!db->doesDataItemExist("cellupvecs")) {
		TVectorData<TVectorData<double>* > *cell_up_vecs  = new TVectorData<TVectorData<double>* > ("cellupvecs", "tvectordata_doublematrix");
		cell_up_vecs->reserveSize(0);
		db->addData(cell_up_vecs->getName(), cell_up_vecs);
	}
	// cellspeeds
	if (!db->doesDataItemExist("cellspeeds")) {
		TVectorData<double> *cellspeeds = new TVectorData<double>("cellspeeds", "tvector_double");
		cellspeeds->reserveSize(0);
		db->addData(cellspeeds->getName(), cellspeeds);
	}

	/// parse the following lines and call the correct methods
	while (getline(*input, line) && line != "END") {
		Util::trim(line);
		if (line.substr(0,1) != "#" && line != "") {
			iss.clear();
			iss.str(line);
			iss >> type;
			if (type == "ALL_SAME_POSITION") {
				// read the position of cells from a file
				double x; double y; double z;
				int number;
				iss >> x; iss >> y; iss >> z;
				iss >> number;
				cpi.initAllInSamePosition(this->ag,x,y,z,number);
			} else if (type == "AROUND_POSITION") {
				// read position and radius
				double x; double y; double z; double r;
				int number;
				iss >> x; iss >> y; iss >> z; iss >> r;
				iss >> number;
//				cerr << x << " " << y << " " << z << " " << r << " " << number << endl;
				cpi.initRandomlyAroundPosition(this->ag,x,y,z,r,number);
			} else if (type == "RANDOM_IN_CUBE") {
				int number;
				iss >> number;
				// nothing to be read
				cpi.initRandomlyInCube(this->ag, number);
			} else if (type == "FROM_FILE") {
				// get filename
				string filename;
				iss >> filename;
				ifstream cellinput(filename.c_str());
				cpi.initFromFile(this->ag,cellinput);
			} else if (type == "FROM_FILE_FOR_MICROFLUIDIC"){
				// get filename
				string filename;
				iss >> filename;
				ifstream input(filename.c_str());
				ifstream geo(name_geometry_input_file_fmf.c_str());
				// call the relevant method.
				cpi.initFromFileForMicroFluidic(this->ag, input, geo);
			} else {
				throw HiveException("do not understand given keyword: '"+line+"'", "GWAParser::parseCellPositions");
			}
		}
	}
	// done parsing the file
}


void GWAParser::parseTimeSteps() {
	string line = "";
	istringstream iss;
	int number;
	double dt;
	double startTime;
	double eqTime;
	ChemoPop::CellPositionInitialiser cpi;
	while (getline(*input, line) && line != "END") {
		Util::trim(line);
		if (line.substr(0,1) != "#" && line != "") {
			iss.clear();
			iss.str(line);
			iss >> number;
			iss >> dt;
			iss >> startTime;
			iss >> eqTime;
			/// set up variables
			IntegerData *nsteps  = new IntegerData("num_steps", number);
			DoubleData *timestep = new DoubleData("dt", dt);
			DoubleData *startt   = new DoubleData("localworldtime", startTime);
			DoubleData *eqt      = new DoubleData("eqTime", eqTime);
			/// add them to the database
			db->addData(nsteps);
			db->addData(timestep);
			db->addData(startt);
			db->addData(eqt);
		}
	}
}

/*
void GWAParser::readCellPositionsForMicroFluidic(string file) {
	ifstream input(file.c_str());
	input.seekg(0,ios::beg);

	ifstream geo(name_geometry_input_file_fmf.c_str());
	geo.seekg(0,ios::beg);

	// some local variables.
	string line = "";
	istringstream iss;
	string line2 = "";
	istringstream iss2;

	string from = ",";
	string to   = " ";
	size_t lookHere;
	size_t foundHere;

	int voxel_index = 0;
	int num_cells   = 0;
	int value       = 0;
	vector<double> coordinate(3);

	// get the relevant data from the database starting with the cell_positions
	// if (db->doesDataItemExist("cellpositions"))
	TVectorData<TVectorData<double>* > *cell_positions =
			(TVectorData<TVectorData<double>* >*) db->getDataItem("cellpositions");
	// get the grid of the environment from the database
	Grid *g = (Grid *) db->getDataItem("environment");
	// get the direction vectors of the cells
	TVectorData<TVectorData<double>* > *cell_dir_vecs =
			(TVectorData<TVectorData<double>* >*) db->getDataItem("celldirvecs");
	// get the up vectors of the cells
	TVectorData<TVectorData<double>* > *cell_up_vecs =
			(TVectorData<TVectorData<double>* >*) db->getDataItem("cellupvecs");
	// get the cellspeed vector
	TVectorData<double> *cell_speeds = (TVectorData<double>*) db->getDataItem("cellspeeds");

	// iterate over the input
	while (getline(input,line)) {
		lookHere = 0;
		// remove comma delimiters and change them to empty spaces
		while ((foundHere = line.find(from, lookHere)) != string::npos) {
			line.replace(foundHere,from.size(),to);
			lookHere = foundHere + to.size();
		}
		// remove comma delimiters from geometry file
		getline(geo,line2);
		lookHere = 0;
		while ((foundHere = line2.find(from, lookHere)) != string::npos) {
			line2.replace(foundHere,from.size(),to);
			lookHere = foundHere + to.size();
		}
		iss.clear();      // cell input
		iss.str(line);
		iss2.clear(); 	  // geometry input
		iss2.str(line2);

		while (iss2 >> value) {
			iss >> num_cells;
			// if we have a real voxel
			if (value == 1) {
				// get the center coordinate of the voxel that we currently are dealing with
				g->getCenterOfVoxel(voxel_index++, coordinate);
	//			cerr << "hier" << endl;
	//			cerr << coordinate[0] << " " << " " << coordinate[1] << " " << coordinate[2] << endl;
				// add cells to the system
				for (int i=0; i<num_cells; i++) {
					// add a cell to the cell positions vector
					TVectorData<double> *pos = new TVectorData<double>("position", "tvectordata_double");
					pos->reserveSize(3);
					pos->at(0) = coordinate[0]; pos->at(1) = coordinate[1]; pos->at(2) = coordinate[2];
					cell_positions->addElementToEnd(pos);
					// do the initialisation of the cell_dir_vecs
					TVectorData<double> *dir = new TVectorData<double>("direction", "tvectordata_double");
					dir->reserveSize(3);
					dir->at(0) = 1; dir->at(1) = 0; dir->at(2) = 0;
					cell_dir_vecs->addElementToEnd(dir);
					// do the initialisation of the cell_up_vecs
					TVectorData<double> *up = new TVectorData<double>("up", "tvector_double");
					up->reserveSize(3);
					up->at(0) = 0; up->at(1) = 1; up->at(2) = 0;
					cell_up_vecs->addElementToEnd(up);
					// do the initialisation of the cell speed vector
					// note that there is currently no way to change the speed of a cell.
					cell_speeds->addElementToEnd(20.0);
				}
			}
		}
	}
	// set the numberofcells to the correct value
	IntegerData *nc = (IntegerData*) db->getDataItem("numberofcells");
	nc->setInteger(cell_positions->size());
}
*/

