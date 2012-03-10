#include "heatbugcomposer.hh"

using namespace Heatbug;

HeatbugComposer::HeatbugComposer() : Composer() {
	throw HiveException("heatbugcomposer called without commandline - i do not know what to do",
						"HeatbugComposer::HeatbugComposer()");
}

HeatbugComposer::HeatbugComposer(int argn, char *argv[]) : Composer() {
	cerr << "# hello, i am the heatbugcomposer" << endl;

	this->dt          = 1;
	this->num_steps   = 0;
	this->number_bugs = 0;

	// read in the command line
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

	// ------------------------------------
	// do the parsing of the command line
	// ------------------------------------
	// check whether the number of bugs has been specified
	if (commandline.find("-numbugs") != commandline.end())
		this->number_bugs = atof(commandline["-numbugs"].c_str());
	else
		this->number_bugs = 100;
	cerr << "# using " << this->number_bugs <<  " bugs for the simulation " << endl;

	// check whether the number of steps has been specified
	if (commandline.find("-numsteps") != commandline.end())
		this->num_steps = atoi(commandline["-numsteps"].c_str());
	else
		this->num_steps = 10000;
	cerr << "# running simulation for " << this->num_steps << endl;

	// check whether an outputsuffix has been specified
	if (commandline.find("-outputsuffix") != commandline.end())
		this->outputSuffix = commandline["-outputsuffix"];
	else
		this->outputSuffix = "";

	// check whether an outputinterval has been specified
	if (commandline.find("-outputinterval") != commandline.end())
		this->output_interval = atof(commandline["-outputinterval"].c_str());
	else
		this->output_interval = 10;
	cerr << "# using an outputinterval of" << this->output_interval << endl;


	// check whether the output is supposed to be in the binary format
	if (commandline.find("-binary") != commandline.end())
	{
		string trueStr = "true";
		if(trueStr.compare(commandline["-binary"])==0)
			binaryOutput = true;
		else
			this->binaryOutput = false;
	}


}

HeatbugComposer::~HeatbugComposer() {
	delete post;
}

int HeatbugComposer::getNumSteps() {
	return this->num_steps;
}

double HeatbugComposer::getTimeIncrement() {
	return this->dt;
}

void HeatbugComposer::setNumberOfBugs(int n) {
	this->number_bugs = n;
}

void HeatbugComposer::setupSimulation() {
	cerr << "# starting to initialise the agent factories" << endl;
	this->initializeAgentFactories();

	Registrar *r = Registrar::getSystemRegistrar();

	cerr << "# creating the maestro" << endl;
	maestro = factories.at(0)->createAgent();
	orchestra.push_back(maestro);
	r->registerNewAgent(maestro);

	cerr << "# creating the bugs" << endl;
	for (int i=0; i<this->number_bugs; i++) {
		Agent *cell = factories[1]->createAgent();
		orchestra.push_back(cell);
		r->registerNewAgent(cell);
	}

	cerr << "# creating the output agent" << endl;
	SpecialAgent *shakespeare = (SpecialAgent* ) factories.at(2)->createAgent();
	special_agents.push_back(shakespeare);
	r->registerNewAgent(shakespeare);
	r->registerNewSpecialAgent(shakespeare);

	cerr << "# setting up the hierarchy among the agents" << endl;
	this->setupAgentHierarchy();

	cerr << "# adding the serial communicator" << endl;
	this->addSerialCommunicator();

	cerr << "# done setting up the heatbug simulation" << endl;
}

void HeatbugComposer::initializeAgentFactories() {
	// create world agent factory
	string line,cellProgram;
	  ifstream cellFile ("cell.py");
	  if (cellFile.is_open()){
	    while ( cellFile.good() ){
	      getline (cellFile,line);
	        cellProgram+=line+"\n";
	    }
	    cellFile.close();
	  }else cout << "Unable to open file"<<endl;

	WorldFactory *wf = new WorldFactory();
	wf->setNumberBugs(this->number_bugs);
	wf->setOutputInterval(this->output_interval);
	factories.push_back(wf);
	cerr << "# done adding world factory" << endl;

	// create bug factory
	BugFactory *bf = new BugFactory();
	bf->setOutputInterval(this->output_interval);
	bf->setProgram(cellProgram);
	factories.push_back(bf);
	cerr << "# done adding bug factory" << endl;

	// create output agent factory
	OutputAgentFactory *oaf = new OutputAgentFactory();
	oaf->setNumberBugs(this->number_bugs);
	oaf->setOutputInterval(this->output_interval);
	oaf->setOutputStyle(this->binaryOutput);
	oaf->setOutputFileNameSuffix(this->outputSuffix);
	factories.push_back(oaf);
	cerr << "# done adding output agent factory" << endl;
}

// straightforward way for setting up the agent hierarchy
// bugs are the children of the maestro and the maestro is the parent of the bugs.
void HeatbugComposer::setupAgentHierarchy() {
	// iterate over all bugs and establish correct links
	for (int i=1;i<=this->number_bugs; i++) {
		Agent *bug = orchestra.at(i);
		maestro->addChild(bug->getAgentId());
		bug->setParent(maestro->getAgentId());
		bug->addSpecialAgent(this->special_agents[0]->getAgentId());
	}
	maestro->addSpecialAgent(this->special_agents[0]->getAgentId());
}

void HeatbugComposer::addSerialCommunicator() {
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

Agent* HeatbugComposer::createAgent(AgentFactory& af) {
	return af.createAgent();
}
