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

#include "core/agent/agent.hh"
#include "core/agent/special_agent.hh"

#include "core/exception/exception.hh"

#include "core/agent/data/primitive/primitiveData.hh"

#include "core/util/util.hh"
#include "core/util/sparselib++/compcol_double.h"
#include "core/util/sparselib++/comprow_double.h"
#include "core/util/sparselib++/coord_double.h"
#include "core/util/sparselib++/ilupre_double.h"
#include "core/util/sparselib++/gmres.h"
#include "core/util/sparselib++/mvm.h"
#include "core/util/sparselib++/mvv.h"

#include "hive.hh"
#include "core/registrar/registrar.hh"


using namespace std;
using namespace Hive;


/**
 *  Main method for hive.
 *

 *  @param argc number of arguments (this is the length of argv)
 *  @param argv[] pointer to char arrays (strings) that were passed as arguments
 *  @return zero if hive ran successfully, an error code if it did not
 *
 *
 */


int main(int argc, char *argv[])
{
	/// everything is inside the exception loop
	try {
		// start the clock so we know how long the simulation takes
		clock_t start,finish;
		string versionNumber = "0.00a2.1 - (very unstable release)";
		cerr << "# entering the hive [v"+versionNumber+"]\n";
		cerr << "# this is the single processor version for chemotaxis and ecology simulations" << endl;
		start=clock();


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




