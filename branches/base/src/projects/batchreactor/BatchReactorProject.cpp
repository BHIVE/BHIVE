/*  ChemotaxisProject.cpp
 *  Created on: 12-21-2010
 *  Author: chenry  */

#include "hive.hh"

using namespace std;
using namespace Hive;
using namespace BatchReactor;

//Global pointer to the project object
Project* Project::currentProject = NULL;

//This function creates the global project object
static Project* Project::createProject() {
	if(Project::currentProject==NULL) {
		Project::currentProject = new Project();
	}
	return Project::currentProject;
};

//This function retrieves the global project object
static Project* Project::getProject() {
	return Project::currentProject;
};

//This function deletes the global project object
static void Project::deleteProject() {
	if(Project::currentProject!=NULL) {
		delete Project::currentProject;
		Project::currentProject = NULL;
	}
};

/// constructor
Project::Project() {};

/// destructor
Project::~Project() {};

//This function is called to run the project. The command line arguments are passed into this function
void Project::runProject(int argc, char *argv[]) {
	/// waiting for the real code
	ChemoPopComposer *siggy = new ChemoPopComposer(argc, argv);
	siggy->setupSimulation();

	Agent *maestro = siggy->getTopLevelAgent();

	int num_steps = siggy->getNumSteps();
	double dt     = siggy->getTimeIncrement();
	// get the output agent
	SpecialAgent *ibn_abi_sarh = siggy->getSpecialAgent(0);

	// wouldn't it be nice to have a progress bar?  the answer is yes. BRILLIANT!
	cerr<<endl<<"# running simulation for "<<num_steps<<" steps, so come back later."<<endl;
	double ticmarks = 40;
	cerr<<endl<<"Progress:\n[----------|--------50---------|---------]"<<endl;
	cerr<<"[";
	double currentPos = 0;

	for (int i=0; i<=num_steps; i++)
	{
		if( (double)i/(double)num_steps > currentPos/ticmarks) {
			cerr<<"+"; currentPos++;
		}
		// actually we wanted to have a propagator class for simulating the system. this is the old way of
		// doing it. needs to be updated.
		maestro->propagate(dt);
		// tell the output agent to evaluate his message queue
		ibn_abi_sarh->evaluateMessageQueue();
	}
	cerr<<"]"<<endl<<endl;

	//Destroy the registrar
	Registrar::deleteSystemRegistrar();
	delete siggy;
};
