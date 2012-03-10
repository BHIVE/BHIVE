/**
 * @file src/hive.cpp
 * @brief The main entry point for hive
 *
 * This file contains the main function and some other things.
 *
 * @author Michael Sneddon
 * @date Started: Oct 7, 2009    Last edited: Oct 23, 2009 by Michael / Chris / Ric and Garrit
 *
 * @todo Fill in the details. Memory cheching!!!
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

#include "util/util.hh"

//#include "util/sparselib++/compcol_double.h"
//#include "util/sparselib++/comprow_double.h"
//#include "util/sparselib++/coord_double.h"
//#include "util/sparselib++/ilupre_double.h"
//#include "util/sparselib++/gmres.h"
//#include "util/sparselib++/mvm.h"
//#include "util/sparselib++/mvv.h"

#include "hive.hh"

#include "registrar/registrar.hh"
#include "projects/simpleCell/simpleCell.hh"
//#include "projects/chemotaxis_population/chemopopcomposer.hh"

using namespace std;
using namespace Hive;
//using namespace ChemoPop;

/**
 *  Main method for hive.
 *

 *  @param argc number of arguments (this is the length of argv)
 *  @param argv[] pointer to char arrays (strings) that were passed as arguments
 *  @return zero if hive ran successfully, an error code if it did not
 */
int main(int argc, char *argv[])
{

	try {
		SimpleCell::runSimpleCell(argc,argv, Hive::mpirank(), Hive::mpisize());
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




