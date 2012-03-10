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

/**
 *  Main method for hive.
 *

 *  @param argc number of arguments (this is the length of argv)
 *  @param argv[] pointer to char arrays (strings) that were passed as arguments
 *  @return zero if hive ran successfully, an error code if it did not
 */
int main(int argc, char *argv[])
{
	int rank, size;
    Hive::parallel_initialise(argc,argv,rank, size);

	try {
		clock_t start,finish;
		if(Hive::mpirank()==0) {
			string versionNumber = "0.00a2 - (unstable release)";
			cerr<<"# entering the hive [v"+versionNumber+"]\n";
		}
		start=clock();

		// the run method gets the size and rank of
		// of this cpu as an argument.
		SimpleCell::runSimpleCell(argc,argv, rank, size);

//		cerr << "here" << endl;

//		finish=clock();
//		double time = (double(finish)-double(start))/CLOCKS_PER_SEC;

//		if(Hive::mpirank()==0) {
//			cerr<<"# leaving the hive.  hive time:  "<<time<<" CPU seconds."<<endl;
//		}
	//	MPI_Barrier();
//		Hive::parallel_finalise();
	} catch (HiveException& e) {
		cout.flush();
		cerr<<"# ERROR! :  "<<e.getFullMessage()<<endl;
        Hive::parallel_finalise();
		exit(1);
	} catch (exception& e) {
		cout.flush();
		cerr<<"# ERROR! :  "<<e.what()<<endl;
        Hive::parallel_finalise();
		exit(2);
	}
	cerr << "DONEDONEDONE" << endl;
//	exit(1);

	Hive::parallel_finalise();
//	exit(1);
	return 0;
}





