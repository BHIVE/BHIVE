/**
 * chemopopcomposer.cc
 *
 */

#include "chemopopcomposer.hh"


using namespace std;
using namespace Hive;
using namespace ChemoPop;

ChemoPopComposer::ChemoPopComposer () {
	this->cells_are_blind_agents = false;
	this->dt = 0;
	this->num_steps = 0;
	this->blockWorldOutput = false;


	cerr << "# calling without command line arguments. i can not tolerate that." << endl;
	exit(1);
}


ChemoPopComposer::ChemoPopComposer(int argn, char *argv[]) {
	cerr << "# hello, i am the chemopopcomposer " << endl;


	/// pointer to cell parameter initialiser
	this->cpi = NULL;

	this->cells_are_blind_agents = false;

	/// we are setting up dt and num_steps here and in the worldagentfactory not good!
	this->dt = 0;
	this->num_steps = 0;

    /// ideally we would have a command line parser for this
	/// parse the command line
	int index = 1;
	string first, second;
	while (index < argn) {
		first = argv[index];
		index++;
		if (index == argn) {
			cerr << "wrong command-line" << endl;
			exit(1);
		}
		second = argv[index];
		commandline[first] = second;
		index++;
	}

	// Indicates that we are reading a cellfile that specifies an e.coli population
	if (commandline.find("-cellfile") != commandline.end()) {
		this->cpi = new CellParameterInitializer(commandline["-cellfile"].c_str());
		this->setNumberOfCells(this->cpi->getNumberCells());
		this->cells_are_blind_agents = false;
		//cerr << this->number_of_cells << endl; exit(1);
	}

	// for now, this is just the blind agent count, but it could be an input file
	// that sets all the parameters of the blind agent.
	if (commandline.find("-blindagentfile") != commandline.end()) {
		this->cpi = new BlindAgentParameterInitializer(commandline["-blindagentfile"].c_str());
		this->setNumberOfCells(this->cpi->getNumberCells());
		this->cells_are_blind_agents = true;
	}

	// all information is specified in a metafile. for the moment this is the way to go!
	if (commandline.find("-metafile") != commandline.end()) {
		// set cell number to zero. real number will be determined when initalising the
		// cell positions
		cerr << "# initialising simulation specified in metafile " << commandline["-metafile"] << endl;
		this->setNumberOfCells(0);
		// open the input meta input file
		ifstream input(commandline["-metafile"].c_str());
		// determine the cell type
		string filename_for_initialiser="";
		this->determineCellType(input, filename_for_initialiser);
		input.close();
		// set up the agent initialiser
		if (this->cells_are_blind_agents)
			this->cpi = new BlindAgentParameterInitializer(filename_for_initialiser.c_str());
		else
			this->cpi = new CellParameterInitializer(filename_for_initialiser.c_str());
	}

	if (commandline.find("-outputinterval") != commandline.end()) {
		this->output_interval = atof(commandline["-outputinterval"].c_str());
	}

	if (commandline.find("-outputsuffix") != commandline.end()) {
		this->outputSuffix = commandline["-outputsuffix"];
	} else {
		this->outputSuffix = "";
	}

	if (commandline.find("-binary") != commandline.end())
	{
		string trueStr = "true";
		if(trueStr.compare(commandline["-binary"])==0) {
			binaryOutput = true;
		} else {
			this->binaryOutput = false;
		}
	}

	// if you pass this flag, output for the world will not be generated
	if (commandline.find("-blockworldoutput") != commandline.end())
	{
		string trueStr = "true";
		if(trueStr.compare(commandline["-blockworldoutput"])==0) {
			blockWorldOutput = true;
		} else {
			this->blockWorldOutput = false;
		}
	}


//	cerr << "done with commandline parsing" << endl;


}

ChemoPopComposer::~ChemoPopComposer() {
	if (this->cpi != NULL)
		delete cpi;
	delete post;
}

void ChemoPopComposer::setNumberOfCells(int num) {
	this->number_of_cells = num;
}


int ChemoPopComposer::getNumSteps()
{
	// in maestro:
	//IntegerData *nsteps  = new IntegerData("num_steps", number);
	//DoubleData *timestep = new DoubleData("dt", dt);
	//DoubleData *startt   = new DoubleData("localworldtime", startTime);
	//DoubleData *eqt      = new DoubleData("eqTime", eqTime);

	num_steps = ((IntegerData *) this->maestro->getDatabase()->getDataItem("num_steps"))->getInteger();
	return num_steps;
}

double ChemoPopComposer::getTimeIncrement()
{
	dt = ((DoubleData *) this->maestro->getDatabase()->getDataItem("dt"))->getDouble();
	return dt;
}




void ChemoPopComposer::setupSimulation() {
	this->initializeAgentFactories();

	Registrar *r = Registrar::getSystemRegistrar();

	maestro = this->createAgent(*factories[0]);
	orchestra.push_back(maestro);
	r->registerNewAgent(maestro);

	// propagate the num_steps and dt of the maestro to the composer
	this->dt = ((DoubleData*) maestro->getDatabase()->getDataItem("dt"))->getDouble();
	this->num_steps = ((IntegerData*) maestro->getDatabase()->getDataItem("num_steps"))->getInteger();

	/// error checking before initialising the number of cells
	if (((WorldAgentFactoryII *) factories[0])->getNumCells() != cpi->getNumberCells()) {

		std::stringstream outx, outy;
		outx << ((WorldAgentFactoryII *) factories[0])->getNumCells(); string x; x = outx.str();
		outy << cpi->getNumberCells(); string y; y = outy.str();

		throw HiveException("this one will be fatal! world factory and cell factories operate with different cell numbers ("+x+" in worldFactory vs. "+y+" in cellFactory)",
				"ChemoPopComposer::setupSimulation()");
	}

	// get the number of cells
	this->number_of_cells = cpi->getNumberCells();

	cerr <<  "# creating cells " << endl;
	// create the cells of the orchestra
	for (int i =0; i<this->number_of_cells; i++) {
		Agent *cell = factories[1]->createAgent();
		orchestra.push_back(cell);
		r->registerNewAgent(cell);
	}
//	cerr << "hier" << endl;
	//maestro->getDatabase()->printDataBase(cerr);
	//orchestra.at(1)->getDatabase()->printDataBase(cerr);


	// set the number of cells that the output agent will have to deal with
	if (!this->cells_are_blind_agents)
		((OutputAgentFactory*) factories.at(2))->setNumberCells(number_of_cells);
	else
		((OutputAgentFactory*) factories.at(2))->setNumberBlindAgents(number_of_cells);
	// tell the output agent whether it will need to output the world grid
	if (((BoolData*) maestro->getDatabase()->getDataItem("is_grid_environment"))->getBool())
		((OutputAgentFactory*) factories.at(2))->setWorldHasGrid(true);
	// create the output agent
	SpecialAgent *ibn_abi_sarh = (SpecialAgent* ) factories.at(2)->createAgent();
	ibn_abi_sarh->setOutputInterval(this->output_interval);
	special_agents.push_back(ibn_abi_sarh);
	r->registerNewAgent(ibn_abi_sarh);
	r->registerNewSpecialAgent(ibn_abi_sarh);

	//ibn_abi_sarh->getDatabase()->printDataBase(cerr);
	//exit(1);

	this->setupAgentHierarchy();

	this->addSerialCommunicator();

	this->num_steps = ((WorldAgentFactoryI*)this->factories.at(0))->getNumSteps();
	this->dt        = ((WorldAgentFactoryI*)this->factories.at(0))->getTimeIncrement();

	// we do not need to send an initialisation messages.
	cerr << "# done setting up the simulation" << endl;

	/// just for testing the new model by will, dumb the cell agent's databases to see that everything
	/// has been initialised correctly.
//	Database *db101 = orchestra[1]->getDatabase();
//	db101->printDataBase(cerr);
//	Database *db202 = orchestra[2]->getDatabase();
//	db202->printDataBase(cerr);

//	exit(1);
}

void ChemoPopComposer::initializeAgentFactories() {
	// in a future version we would have to open the worldfile and cehck which factory needs to be built.

	// initialise the world agent factory ... old factories with parser build in
	/*
	this->factories.push_back(new WorldAgentFactoryI());
	this->factories.at(factories.size()-1)->finalise();

	// this ...
	((WorldAgentFactoryI *)this->factories.at(factories.size()-1))->setNumberCells(number_of_cells);
	((WorldAgentFactoryI *)this->factories.at(factories.size()-1))->setInput(commandline["-worldfile"]);
	((WorldAgentFactoryI *)this->factories.at(factories.size()-1))->setOutputInterval(this->output_interval);
	((WorldAgentFactoryI *)this->factories.at(factories.size()-1))->setBlockWorldOutput(blockWorldOutput);
    */

	// initialise the world agent factory: factory which uses an external parser.
	this->factories.push_back(new WorldAgentFactoryII());
	this->factories.at(factories.size()-1)->finalise();
	// provide the world agent factory with the correct input file
	((WorldAgentFactoryII *)this->factories.at(factories.size()-1))->setNumberCells(number_of_cells);
	if (commandline.find("-metafile") != commandline.end())
		((WorldAgentFactoryII *) this->factories.at(factories.size()-1))->setInput(commandline["-metafile"]);
	else
		((WorldAgentFactoryII *)this->factories.at(factories.size()-1))->setInput(commandline["-worldfile"]);
	// set the output interval which is given by the commandline
	((WorldAgentFactoryII *)this->factories.at(factories.size()-1))->setOutputInterval(this->output_interval);
	// passing flaging indicating whether output for the world is to be blocked or not
	((WorldAgentFactoryII *)this->factories.at(factories.size()-1))->setBlockWorldOutput(blockWorldOutput);
	// propagate information of whether we are dealing with blind agents to the world factory
	if(this->cells_are_blind_agents) {
		((WorldAgentFactoryII *)this->factories.at(factories.size()-1))->setIsBlindAgentCells(true);
	} else {
		((WorldAgentFactoryII *)this->factories.at(factories.size()-1))->setIsBlindAgentCells(false);
	}

	// initialise the proper cell factory
	if(this->cells_are_blind_agents) {
		// for blind agents
		this->factories.push_back(new BlindAgentFactory());
		((BlindAgentFactory*)this->factories.at(factories.size()-1))->setInitializer((CellParameterInitializer*)this->cpi);
		((BlindAgentFactory*)this->factories.at(factories.size()-1))->setOutputInterval(this->output_interval);
		this->factories.at(factories.size()-1)->finalise();
	} else {
		// for chemotaxis cells
		this->factories.push_back(new CellAgentFactory());
		((CellAgentFactory*)this->factories.at(factories.size()-1))->setInitializer((CellParameterInitializer*) this->cpi);
		((CellAgentFactory*)this->factories.at(factories.size()-1))->setOutputInterval(this->output_interval);
		this->factories.at(factories.size()-1)->finalise();
	}

	// initialise the outputagent factory
	// create output agent factory.
	this->factories.push_back(new OutputAgentFactory());
	// prepare as much of the output agent factory as possible
	//	cerr << this->output_interval << endl;
	((OutputAgentFactory *)this->factories.at(factories.size()-1))->setOutputInterval(this->output_interval);
	((OutputAgentFactory *)this->factories.at(factories.size()-1))->setOutputStyle(this->binaryOutput);
	((OutputAgentFactory *)this->factories.at(factories.size()-1))->setOutputFileNameSuffix(this->outputSuffix);
	this->factories.at(factories.size()-1)->finalise();

	//Register all the factories
	for(int k=0; k<factories.size(); k++)
	{
		Registrar::getSystemRegistrar()->registerAgentFactory(factories.at(k));
	}
}

void ChemoPopComposer::setupAgentHierarchy() {
	// set children of maestro
	if (this->number_of_cells > 0) {
		for (int i=1;i<=this->number_of_cells; i++) {
			Agent *cell = orchestra.at(i);
			maestro->addChild(cell->getAgentId());
			cell->setParent(maestro->getAgentId());
			cell->addSpecialAgent(this->special_agents[0]->getAgentId());
		}
	}
	maestro->addSpecialAgent(this->special_agents[0]->getAgentId());
}

void ChemoPopComposer::addSerialCommunicator() {
	post = new SerialCommunicator();
	for (unsigned int i=0; i<orchestra.size(); i++) {
		post->addAgent(orchestra[i]);
		orchestra[i]->addCommunicator(post);
	}

	for (unsigned int i=0; i<this->special_agents.size(); i++) {
		post->addAgent(special_agents[i]);
		special_agents[i]->addCommunicator(post);
	}

}

Agent* ChemoPopComposer::createAgent(AgentFactory& af) {
	return af.createAgent();
}

void ChemoPopComposer::determineCellType(ifstream& input, string& filename_for_initialiser) {
	// some local variables
	string line = "";
	istringstream iss;
	string firstword;
	string secondword;
	string thirdword;
	// read the file
	while (getline(input,line)) {
		Util::trim(line);
		iss.clear();
		iss.str(line);
		iss >> firstword;
		// look for CELLTYPE key
		if (firstword == "CELLTYPE") {
			iss >> secondword;
			iss >> thirdword;
			// set the flag
			if (secondword == "ecoli" || secondword == "ECOLI" || secondword == "Ecoli" || secondword == "EColi")
				this->cells_are_blind_agents = false;
			else
				this->cells_are_blind_agents = true;
			filename_for_initialiser = thirdword;
		}
	}
}
