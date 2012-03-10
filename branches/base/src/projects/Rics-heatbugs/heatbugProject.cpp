#include "../../hive.hh"
#include "../project.hh"

#include "heatbugcomposer.hh"

using namespace std;
using namespace Hive;
using namespace Heatbug;

/// constructor
Project::Project() {};

/// destructor
Project::~Project() {};

void Project::runProject(int argc, char *argv[]) {
	HeatbugComposer *siggy = new HeatbugComposer(argc, argv);
	siggy->setupSimulation();

	Agent *maestro = siggy->getTopLevelAgent();

	int num_steps = siggy->getNumSteps();
	double dt     = siggy->getTimeIncrement();
	SpecialAgent *ibn_abi_sarh = siggy->getSpecialAgent(0);

	// the good old progress bar
	cerr<<endl<<"# running simulation for "<<num_steps<<" steps, so come back later."<<endl;
	double ticmarks = 40;
	cerr<<endl<<"Progress:\n[----------|--------50---------|---------]"<<endl;
	cerr<<"[";
	double currentPos = 0;

	for (int i=0; i<num_steps; i++)
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

	delete siggy;
};
