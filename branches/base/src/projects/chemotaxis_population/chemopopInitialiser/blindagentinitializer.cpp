#include "blindagentinitializer.hh"

using namespace std;
using namespace Hive;
using namespace ChemoPop;

BlindAgentParameterData::BlindAgentParameterData() { }

BlindAgentParameterData::~BlindAgentParameterData() { }

BlindAgentParameterInitializer::BlindAgentParameterInitializer() {
	number_cells = 0;
	currentCell  = 0;
}


BlindAgentParameterInitializer::BlindAgentParameterInitializer(string name) {
	this->number_cells = 0;
	currentCell = 0;
	this->parseFile(name);
}


BlindAgentParameterInitializer::~BlindAgentParameterInitializer() {
	/// does it call the destructor of the super class ?! i don't think so ...

	for(unsigned int k=0; k<uniqueBAPDs.size(); k++) {
		ParameterData * data = uniqueBAPDs.at(k);
		if(data!=NULL)
			delete data;
	}
}


void BlindAgentParameterInitializer::parseFile(string name) {
	ifstream distFile(name.c_str());
	string line = "";
	istringstream iss;

	/// local variables for storing values from the parsed file
	string type;
	double v;
	double cost_mov;
	double kcat;
	double km;
	double yield;
	double death_thres;
	double initial_essence;
	double spontaneous_death_rate;
	double rho;
	bool   is_levy_distribution;
	double distribution_parameter;
	double birth_threshold;
	int    num;
	double effeciency;
	double r1;

	// to add:

	//  1)  background death rate

	//  2)  rho for change in angle (must be between 0 and 1)

	//  3) 0 or 1 to decide between exponential or levy

	//  4) movement parameter (either Beta or gamma)
	//      - beta for levy flight, gamma for exponential

	// For levy walk, sample using this function
	//lmax = 1000000;
	//lmin = 1;

	//beta = -1.1;

	//L = (((lmax.^(beta+1) - lmin.^(beta+1)).*Y+lmin.^(beta+1)).^(1./(beta+1)));

	bool verbose = true;
	if(verbose) cerr<<"BLIND AGENT INITIALIZER PARSING FILE:"<<endl;

	// parse the file
	while(getline(distFile,line)) {
		// check that line is not an empty line and that it is not a comment
		if (line.substr(0, 1)!="#" && line != "" && line.substr(0,1)!="%") {

			if(verbose) cerr<<"[line]: "<<line<<endl;

			 try {
				 iss.clear();
				 iss.str(line);
				 // read all the parameters
				 iss >> type;
				 iss >> v;
				 iss >> cost_mov;
				 iss >> kcat;
				 iss >> km;
				 iss >> yield;
				 iss >> death_thres;
				 iss >> initial_essence;
				 iss >> spontaneous_death_rate;
				 iss >> rho;
				 iss >> is_levy_distribution;
				 iss >> distribution_parameter;
				 iss >> effeciency;
				 iss >> r1;
				 iss >> birth_threshold;
				 iss >> num;
			 } catch (...) {
				 throw HiveException("error in parsing blind agent parameter file",
				 				"BlindAgentParameterInitializer::parseFile(string name)");
			 }
			 // put them into the correct data
			 BlindAgentParameterData *bapd        = new BlindAgentParameterData();
			 bapd->type                           = type;
			 bapd->velocity                       = v;
			 bapd->cost_for_movement              = cost_mov;
			 bapd->kcat                           = kcat;
			 bapd->Km                             = km;
			 bapd->yield                          = yield;
			 bapd->death_thres                    = death_thres;
			 bapd->initial_essence                = initial_essence;
			 bapd->spontaneous_death_rate         = spontaneous_death_rate;
			 bapd->rho							  = rho;
			 bapd->is_levy                        = is_levy_distribution;
			 bapd->effeciency                     = effeciency;
			 bapd->r1                             = r1;
			 bapd->birth_thres                    = birth_threshold;
			 bapd->parameter_for_distribution     = distribution_parameter;


			 // error checking here
			 if(verbose) {
				 cerr<<" === type:                        "<<bapd->type<<endl;
				 cerr<<" === velocity:                    "<<bapd->velocity<<endl;
				 cerr<<" === cost_for_movement:           "<<bapd->cost_for_movement<<endl;
				 cerr<<" === max growth rate:             "<<bapd->kcat<<endl;
				 cerr<<" === Km:                          "<<bapd->Km<<endl;
				 cerr<<" === yield:                       "<<bapd->yield<<endl;
				 cerr<<" === death_thres:                 "<<bapd->death_thres<<endl;
				 cerr<<" === initial_essence:             "<<bapd->initial_essence<<endl;
				 cerr<<" === spontaneous_death_rate:      "<<bapd->spontaneous_death_rate<<endl;
				 cerr<<" === rho:                         "<<bapd->rho<<endl;
				 cerr<<" === is_levy:                     "<<bapd->is_levy<<endl;
				 cerr<<" === param_for_run_lenght_dist:   "<<bapd->parameter_for_distribution<<endl;
				 cerr<<" === effeciency                   "<<bapd->effeciency<<endl;
				 cerr<<" === r1                           "<<bapd->r1<<endl;
				 cerr<<" === birth threshold:             "<<bapd->birth_thres<<endl;
				 cerr<<" === number of cells:             "<<num<<endl;
			 }

			 if(bapd->velocity<0 || bapd->velocity >1)
				 throw HiveException( "velocity must be between 0 and 1 inclusive.",
				 				 	  "BlindAgentParameterInitializer::parseFile(string name)");



			 // fill up cells vector
			 uniqueBAPDs.push_back(bapd);
			 for (int i=0; i<num; i++)
				 cells.push_back(bapd);
			 // do the cell number counting
			 this->number_cells += num;
		 }
	}


	this->setNumberOfCells(number_cells);
	if(verbose) cerr<<"total cells to generate: "<<this->getNumberCells()<<endl;
	if(verbose) cerr<<"FINISHED PARSED"<<endl<<endl;
}


void BlindAgentParameterInitializer::setNextCellParameters(Agent *cell) {
	// error checking
	if (currentCell >= this->number_cells) {

		// No longer an error, because cells are created during the simulation and copied from
		// existing parameters, so you will get here during a simulation even after all your
		// initial cells have been generated.
		return;

		//	cerr<<"currentCell "<<currentCell<<endl;
		//	cerr<<"number of cells: "<<this->number_cells<<endl;
		//	cerr<<"vector size: "<<this->cells.size()<<endl;
		//	throw HiveException("# cannot set parameters for another blind agent, all parameters have been already used",
		//			"BlindAgentParameterInitializer::setNextCellParameters(Agent *cell)");
	}
	// now set the values of the data items in the blind agent's database
	// note that we get the correct data from the blind agents database. that
	// rises the question as to why we really bothered to put the stuff up there
	// in the first place. ... we might reconsider this.
	BlindAgentParameterData *bapd = (BlindAgentParameterData*) cells.at(currentCell);
	Database *db = cell->getDatabase();

//	StringData *mytypename = (StringData*) db->getDataItem("mytypename");
//	mytypename->setString(bapd->type);

	DoubleData *velo   = (DoubleData* ) db->getDataItem("velocity");
	velo->setDouble(bapd->velocity);

	DoubleData *kcat   = (DoubleData* ) db->getDataItem("kcat");
	kcat->setDouble(bapd->kcat);

	DoubleData *Km     = (DoubleData* ) db->getDataItem("Km");
	Km->setDouble(bapd->Km);

	DoubleData *yield  = (DoubleData* ) db->getDataItem("yield");
	yield->setDouble(bapd->yield);

	DoubleData *cost_for_movement = (DoubleData* ) db->getDataItem("essence_cost_for_movement");
	cost_for_movement->setDouble(bapd->cost_for_movement);

	DoubleData *death_threshold = (DoubleData*) db->getDataItem("essence_threshold_for_death");
	death_threshold->setDouble(bapd->death_thres);

	DoubleData *birth_threshold = (DoubleData*) db->getDataItem("essence_threshold_for_birth");
	birth_threshold->setDouble(bapd->birth_thres);

	DoubleData *essence = (DoubleData*) db->getDataItem("essence");
	essence->setDouble(bapd->initial_essence);

	DoubleData *spon_death_rate = (DoubleData*) db->getDataItem("background_death_rate_per_unit_time");
	spon_death_rate->setDouble(bapd->spontaneous_death_rate);

	DoubleData *rho = (DoubleData*) db->getDataItem("rho");
	rho->setDouble(bapd->rho);

	BoolData *is_levy = (BoolData*) db->getDataItem("is_levy");
	is_levy->setBool(bapd->is_levy);

	DoubleData *effeciency = (DoubleData*) db->getDataItem("base_effeciency_of_conversion");
	effeciency->setDouble(bapd->effeciency);

	DoubleData *r1 = (DoubleData*) db->getDataItem("r1");
	r1->setDouble(bapd->r1);

	DoubleData *param = (DoubleData*) db->getDataItem("parameter_for_steplength_dist");
	param->setDouble(bapd->parameter_for_distribution);

	// increase counter
	currentCell += 1;
}

