/**
 * @file src/hive.hh
 * @brief The main header file for hive
 *
 * This file contains the main function headers and declarations.
 *
 * @author Michael Sneddon
 * @date Started: Oct 14, 2009    Last edited: Oct 14, 2009
 *
 * @todo Fill in the details.
 *
 */
#ifndef HIVE_HH_
#define HIVE_HH_

//!  Core Hive classes and functionality.
/*!
 *  All of the Hive framework classes and methods are defined in this namespace,
 *  so you should use this namespace in all of your projects.  Be sure to define
 *  a new namespace for problem specific projects.
 *  @author Michael Sneddon
 */

namespace Hive {
	class global_data
	global_data* getGlobalData();
	int initialize(int argc, char *argv[]);
	void finalize(string message, int exitCode);
	void abort(string message, int exitCode);
}


#endif /* HIVE_HH_ */
