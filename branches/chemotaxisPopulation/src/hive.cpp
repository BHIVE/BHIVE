/**
 * @file src/hive.cpp
 * @brief The main entry point for hive
 *
 * This file contains the main function and some other things.
 *
 * @author Michael Sneddon
 * @date Started: Oct 7, 2009    Last edited: Oct 23, 2009 by Michael / Chris / Ric and Garrit
 *
 * @todo Fill in the details. Memory checking!!! we always have to do that ...
 *
 */

#include <iostream>
#include <exception>
#include <string>
#include <time.h>
#include <fstream>

#include <map>

#include "agent/agent.hh"
#include "agent/special_agent.hh"

#include "exception/exception.hh"

#include "agent/data/primitive/primitiveData.hh"
#include "agent/data/grid/grid.hh"
#include "agent/data/grid/quadtreeformicrofluidic.hh"

#include "util/util.hh"
#include "util/sparselib++/compcol_double.h"
#include "util/sparselib++/comprow_double.h"
#include "util/sparselib++/coord_double.h"
#include "util/sparselib++/ilupre_double.h"
#include "util/sparselib++/gmres.h"
#include "util/sparselib++/mvm.h"
#include "util/sparselib++/mvv.h"

#include "hive.hh"
#include "registrar/registrar.hh"
#include "projects/chemotaxis_population/chemopopcomposer.hh"
#include "input/meshinputreader/gridreaderformicrofluidic.hh"


using namespace std;
using namespace Hive;
using namespace ChemoPop;

/**
 *  Main method for hive.
 *

 *  @param argc number of arguments (this is the length of argv)
 *  @param argv[] pointer to char arrays (strings) that were passed as arguments
 *  @return zero if hive ran successfully, an error code if it did not
 *
 * to run the current version use the following command
 * /home/.../chemoPop -ouputSuffix _whatever -worldfile path -cellfile path -outputinterval somenumber -binary true/false
 *
 * ATTENTION
 * instead of the flag -cellfile one can also pass a flag -blindagentfile path such that information about
 * blind agents is read.
 *
 * I will change this. Instead of passing the correct file there will only be a single file containing all
 * relevant information.
 *
 */

int QuadTreeNodeForMicroFluidic::used_ids = 0;

int main(int argc, char *argv[])
{
	/// everything is inside the exception loop
	try {
		// start the clock so we know how long the simulation takes
		clock_t start,finish;
		string versionNumber = "0.00a2.1 - (very unstable release)";
		cerr<<"# entering the hive [v"+versionNumber+"]\n";
		cerr << "# this is the single processor version for chemotaxis and ecology simulations" << endl;
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


		//Registrar *r = Registrar::getSystemRegistrar();
		//DoubleData *totalNutrient = (DoubleData *)(r->getAgent(0)->getDatabase()->getDataItem("total_nutrient"));

		// wouldn't it be nice to have a progress bar?  the answer is yes. BRILLIANT!
		cerr<<endl<<"# running simulation for "<<num_steps<<" steps, so come back later."<<endl;
		double ticmarks = 40;
		cerr<<endl<<"Progress:\n[----------|--------50---------|---------]"<<endl;
		cerr<<"[";
		double currentPos = 0;

		// num_steps = 100;

		for (int i=0; i<=num_steps; i++)
		{
			if( (double)i/(double)num_steps > currentPos/ticmarks) {
				cerr<<"+"; currentPos++;
			}

			//cerr<<"stepping "<<i<<" of" <<num_steps<<endl;
			//cerr<<"+ "<<i*dt<<" "<<totalNutrient->getDouble()<<" "<<r->getNumberOfAgents()-2<<endl;

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

		// wrap up and output the runtime
		finish=clock();
		double time = (double(finish)-double(start))/CLOCKS_PER_SEC;
		cerr<<"# leaving the hive.  hive time:  "<<time<<" CPU seconds."<<endl;



	} catch (HiveException& e) {
		cout.flush();
		cerr<<"# ERROR! :  "<<e.getFullMessage()<<endl;
		exit(1);
	} catch (exception& e) {
		cout.flush();
		cerr<<"# ERROR! :  "<<e.what()<<endl;
		exit(2);
	}

	return 0;
}




