/*  ChemotaxisProject.cpp
 *  Created on: 12-21-2010
 *  Author: chenry  */

#include "../../hive.hh"
#include "../project.hh"

#include "chemopopcomposer.hh"
#include "chemopopInput/meshinputreader/gridreaderformicrofluidic.hh"
#include "chemopopData/grid/grid.hh"
#include "chemopopData/grid/quadtreeformicrofluidic.hh"
#include "../../core/input/dataParser/dataParser.hh"


using namespace std;
using namespace Hive;
using namespace ChemoPop;

/// constructor
Project::Project() {};


/// destructor
Project::~Project() {};

//This function is called to run the project. The command line arguments are passed into this function
void Project::runProject(int argc, char *argv[])
{
//	DataParser *dp = new DataParser();
//	Agent *a = new Agent();
//	dp->parse("dataTest.txt",a,true);
//	cerr<<"\n\n==done."<<endl;
//	return;

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

	// call a final function that writes out some trajectories before tumble events if it was so requested
	siggy->outputTrajectoriesBeforeTumblesIfRequested();



	//Destroy the registrar
	Registrar::deleteSystemRegistrar();
	delete siggy;
};
