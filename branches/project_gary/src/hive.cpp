/**
 * @file src/hive.cpp
 * @brief The main entry point for hive
 * This file contains the main function and some other things.
 * @author Michael Sneddon
 * @date Started: Oct 7, 2009    Last edited: Oct 23, 2009 by Michael / Chris / Ric and Garrit
 */
#include <stdlib.h>
#include <exception>
#include <time.h>
#include <string>
#include "global_data.h"
#include "parallel/parallel.hh"
#include "hive.hh"

using namespace std;
using namespace Hive;

global_data* global_data_handler;

/**
 *  Main method for hive.
 *  @param argc number of arguments (this is the length of argv)
 *  @param argv[] pointer to char arrays (strings) that were passed as arguments
 *  @return zero if hive ran successfully, an error code if it did not
 */
int main(int argc, char *argv[])
{
    initialize(argc, argv);
	try {
		// start the clock so we know how long the simulation takes
		clock_t start,finish;
		string versionNumber = "0.00a2 - (unstable release)";
		cerr<<"# entering the hive [v"+versionNumber+"]\n";
		start=clock();

		// create the Registrar that keeps track of global system information
		Registrar::createSystemRegistrar();


		/// waiting for the real code
		ChemoPopComposer *siggy = new ChemoPopComposer(argc, argv);
		siggy->setupSimulation();

		Agent *maestro = siggy->getTopLevelAgent();

		int num_steps = siggy->getNumSteps();
		double dt     = siggy->getTimeIncrement();
		// get the output agent
		SpecialAgent *ibn_abi_sarh = siggy->getSpecialAgent(0);


		cerr<<" # running simulation, so come back later."<<endl;
		Registrar *r = Registrar::getSystemRegistrar();
		DoubleData *totalNutrient = (DoubleData *)(r->getAgent(0)->getDatabase()->getDataItem("total_nutrient"));

		for (int i=0; i<num_steps; i++) {

			//cerr<<"stepping "<<i<<endl;
			cout<<"+ "<<i*dt<<" "<<totalNutrient->getDouble()<<" "<<r->getNumberOfAgents()-2<<endl;

			maestro->propagate(dt);
			// tell the output agent to evaluate his message queue
			ibn_abi_sarh->evaluateMessageQueue();

		}

		//Destroy the registrar
		Registrar::deleteSystemRegistrar();
		delete siggy;

		// wrap up and output the runtime
		finish=clock();
		double time = (double(finish)-double(start))/CLOCKS_PER_SEC;
		cerr<<"# leaving the hive.  hive time:  "<<time<<" CPU seconds."<<endl;
		SimpleCell::runSimpleCell(argc,argv, Hive::mpirank(), Hive::mpisize());
	} catch (HiveException& e) {
		cout.flush();
		cerr<<"# ERROR! :  "<<e.getFullMessage()<<endl;
        finalize(e.getFullMessage(),2);
	} catch (exception& e) {
		cout.flush();
		cerr<<"# ERROR! :  "<<e.what()<<endl;
        finalize(e.what(),2);
	}
	finalize("DONEDONEDONE", 0);
	exit(0);
}



int Hive::initialize(int argc, char *argv[]) {
	int rank, size;
	parallel_initialize(argc, argv, rank, size);
	global_data_handler = new global_data();
	return global_data_handler->initialize(argc, argv);
}

void Hive::finalize(string message,int exitCode) {
	cout.flush();
	cerr << message << endl;
	delete global_data_handler;
	Hive::parallel_finalize();
	exit(exitCode);
}

void Hive::abort(string message, int exitCode) {
    Hive::parallel_abort(exitCode, message);
}