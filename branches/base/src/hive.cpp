/**
 * @file src/hive.cpp
 * @brief The main entry point for hive
 *
 * This file contains the main function and some other things.
 *
 * @author Michael Sneddon
 * @date Started: Oct 7, 2009    Last edited: Nov, 2011 by Michael
 *
 * @todo Fill in the details. Memory checking!!! we always have to do that ...
 *
 */

// CDT PARSER IN ECLIPSE DOES NOT RECOGNIZE CLOCKS_PER_SEC, SO
// THIS OVERWRITES THE GENERATED SYNTAX ERROR
#ifdef __CDT_PARSER__
#define CLOCKS_PER_SEC
#endif

#include "hive.hh"

using namespace std;
using namespace Hive;



//Global pointer to the project object
static Project *currentProject = NULL;

//This function creates the global project object
Project* createProject() {
	if(currentProject==NULL) {
		currentProject = new Project();
	}
	return currentProject;
};

//This function retrieves the global project object
Project* getProject() {
	return currentProject;
};

//This function deletes the global project object
void deleteProject() {
	if(currentProject!=NULL) {
		delete currentProject;
		currentProject = NULL;
	}
};


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
 *
 */
int main(int argc, char *argv[])
{
	/// everything is inside the exception loop
	try {
		// start the clock so we know how long the simulation takes
		clock_t start,finish;
		string versionNumber = "0.00a4.1 - (marginally unstable release)";
		cerr<<"# entering the hive [v"+versionNumber+"]\n";
		start=clock();
		// create the Registrar that keeps track of global system information
		// i am not sure whether this thing will really be used.
		Registrar::createSystemRegistrar();
		//creating the project object using the project specific implementation of the project class
		Project* myProject = createProject();
		myProject->runProject(argc,argv);
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


