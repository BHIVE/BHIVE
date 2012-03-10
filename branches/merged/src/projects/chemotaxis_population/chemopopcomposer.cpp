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

	this->cpi = NULL;
	this->cells_are_blind_agents = false;
	this->dt = 0;
	this->num_steps = 0;


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





}

ChemoPopComposer::~ChemoPopComposer() {
	if (this->cpi != NULL)
		delete cpi;
	delete post;
}

void ChemoPopComposer::setNumberOfCells(int num) {
	this->number_of_cells = num;
}

void ChemoPopComposer::setupSimulation() {
	this->initializeAgentFactories();


	Registrar *r = Registrar::getSystemRegistrar();

	maestro = this->createAgent(*factories[0]);
	orchestra.push_back(maestro);
	r->registerNewAgent(maestro);

	cerr<<"number of cells: "<<this->number_of_cells<<endl;
	for (int i =0; i<this->number_of_cells; i++) {
		Agent *cell = factories[1]->createAgent();
		orchestra.push_back(cell);
		r->registerNewAgent(cell);
	}

	//maestro->getDatabase()->printDataBase(cerr);
	//orchestra.at(1)->getDatabase()->printDataBase(cerr);



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

}

void ChemoPopComposer::initializeAgentFactories() {
	// in a future version we would have to open the worldfile and cehck which factory needs to be built.

	// initialise the world agent factory
	this->factories.push_back(new WorldAgentFactoryI());
	this->factories.at(factories.size()-1)->finalise();

	// this ...
	((WorldAgentFactoryI *)this->factories.at(factories.size()-1))->setNumberCells(number_of_cells);
	((WorldAgentFactoryI *)this->factories.at(factories.size()-1))->setInput(commandline["-worldfile"]);
	((WorldAgentFactoryI *)this->factories.at(factories.size()-1))->setOutputInterval(this->output_interval);
	((WorldAgentFactoryI *)this->factories.at(factories.size()-1))->setBlockWorldOutput(blockWorldOutput);


	if(this->cells_are_blind_agents) {
		((WorldAgentFactoryI *)this->factories.at(factories.size()-1))->setIsBlindAgentCells(true);
	} else {
		((WorldAgentFactoryI *)this->factories.at(factories.size()-1))->setIsBlindAgentCells(false);
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
	this->factories.push_back(new OutputAgentFactory());
	((OutputAgentFactory *)this->factories.at(factories.size()-1))->setNumberCells(number_of_cells);
	((OutputAgentFactory *)this->factories.at(factories.size()-1))->setNumberBlindAgents(number_of_cells);
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
	for (int i=1;i<=this->number_of_cells; i++) {
		Agent *cell = orchestra.at(i);
		maestro->addChild(cell->getAgentId());
		cell->setParent(maestro->getAgentId());
		cell->addSpecialAgent(this->special_agents[0]->getAgentId());
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



