/*  ChemotaxisProject.cpp
 *  Created on: 12-21-2010
 *  Author: chenry  */

#include "../../hive.hh"
#include "../project.hh"

#include "core/transwellcomposer.hh"
#include "../../core/input/dataParser/dataParser.hh"


using namespace std;
using namespace Hive;
using namespace Transwell;

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


	// sets the seed
	if (argc == 2) {
		int hans = atoi(argv[1]);
		Util::SEED_RANDOM(hans);
	} else
		Util::SEED_RANDOM(513214);

	Composer *comp = new Transwell::TranswellComposer();

	//  ((Transwell::TranswellComposer *)comp)->setupNumberOfCells(782);

	((Transwell::TranswellComposer *) comp)->setNumberOfTeffCells(782);
	((Transwell::TranswellComposer *) comp)->setNumberOfTregCells(782);

	// generate a simulation
	comp->setupSimulation();
	// get the maestro
	cerr << "# getting the maestro ... " << endl;
	Agent *maestro = comp->getTopLevelAgent();
	SpecialAgent *output_agent = comp->getSpecialAgent(0);
	output_agent->evaluateMessageQueue();
	// run the simulation
	cerr << "# propagating for ... " << endl;
	for (int i=0; i< 3600*60; i++) {  // 3600*60.; i++) {
		cerr << i*1. << endl;
		// broadcast the time step to the agents
		comp->setTargetTime(1.0);
		maestro->propagate(1.0);
		output_agent->evaluateMessageQueue();
	}
	delete comp;


};
