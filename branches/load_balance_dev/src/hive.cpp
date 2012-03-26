/**
 * @file src/hive.cpp
 * @brief The main entry point for hive
 *
 * This file contains the main function and some other things.
 *
 * @author Michael Sneddon
 * @date Started: Oct 7, 2009    Last edited: Oct 23, 2009 by Michael / Chris / Ric and Garrit
 *
 * @todo Fill in the details. Memory cheching!!! DONE!
 *
 */

#include <iostream>
#include <exception>
#include <string>
#include <time.h>
#include <fstream>
#include <map>

#include "agent/agent.hh"
#include "exception/exception.hh"

#include "agent/data/primitive/primitiveData.hh"

// simpleCell model for running capillary and multiple flagella simulations
#include "projects/simpleCell/simpleCell.hh"
#include "parallel/parallel.hh"

#include "hive.hh"

using namespace std;
using namespace Hive;

int michael = 0;
PropagatorClass* currentPropagator = NULL;
ofstream outputStream;

/**
 *  Main method for hive.
 *

 *  @param argc number of arguments (this is the length of argv)
 *  @param argv[] pointer to char arrays (strings) that were passed as arguments
 *  @return zero if hive ran successfully, an error code if it did not
 */
int main(int argc, char *argv[])
{
    initialize(&argc, &argv);
	try {
		clock_t start,finish;
		if(Hive::mpirank()==0) {
			string versionNumber = "0.00a2 - (unstable release)";
			cerr<<"# entering the hive [v"+versionNumber+"]\n";
		}
		start=clock();
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

PropagatorClass* Hive::get_propagator() {
	return currentPropagator;
}

void Hive::set_propagator(PropagatorClass* inPropagator) {
	currentPropagator = inPropagator;
}

void Hive::openOutputStream(string filename) {
	outputStream.open(filename.data());
}

void Hive::closeOutputStream() {
	outputStream.close();
}

ofstream& Hive::getOutputStream() {
	return outputStream;
}

void Hive::initialize(int* argc, char**argv[]) {
	int rank, size;
	parallel_initialize(argc, argv, rank, size);
	if (Hive::mpirank() == 0) {
		openOutputStream("output.txt");
		getOutputStream() << "t,id,node,x,y,z,D,rgb" << endl;
	}
}

void Hive::finalize(string message,int exitCode) {
	cout.flush();
	cerr << message << endl;
	if (Hive::mpirank() == 0) {
		closeOutputStream();
	}
	Hive::parallel_finalize();
	exit(exitCode);
}

void Hive::abort(string message, int exitCode) {
    Hive::parallel_abort(exitCode, message);
}
