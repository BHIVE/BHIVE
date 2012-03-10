/**
 * @file commandLineParser.hh
 *
 * Functions to parse command line arguments into a general map that can be
 * accessed later.
 *
 * @date Oct 14th, 2009   last edited: Oct 14th, 2009
 *
 * @author Michael Sneddon
 */


#ifndef COMMANDLINEPARSER_HH_
#define COMMANDLINEPARSER_HH_


#include "../../util/util.hh"

#include <map>
#include <string>

using namespace std;

namespace Hive
{

	//! Parses command line arguments from the console into an argMap.
	/*!
	 * Given the vector of strings taken from the command line, this function
	 * parses out all strings that start with a dash and identifies them as
	 * parameters, and attatches the parameter value to whatever string follows.
	 * For example, -file help.txt would add an entry to the argMap as a
	 * parameter named "file" with value "help.txt".  You can take a look
	 * at parseAsInt and parseAsDouble functions that can interpret the value
	 * as integers or doubles
	 *
	 * @param argc the number of arguments
	 * @param argv the array of character arrays (strings)
	 * @param argMap the map that will be set when this function is called
	 * @return bool true if successful, false if something went wrong
	 * @author Michael Sneddon
	 */
	bool parseArguments(int argc, char *argv[], std::map<std::string,std::string> &argMap);


	//! Looks up the argument in the argMap and tries to parse the value as an integer
	/*!
	 * @param argMap the argMap to lookup, generally created by the parseArguments function
	 * @param string the name of the parameter to look up
	 * @param defaultValue the default value to return if the value was empty
	 * @return int the parsed int if successful, otherwise the defaultValue given
	 * @author Michael Sneddon
	 */
	int parseAsInt(map<string,string> &argMap, string argName, int defaultValue);


	//! Looks up the argument in the argMap and tries to parse the value as a double
	/*!
	 * @param argMap the argMap to lookup, generally created by the parseArguments function
	 * @param string the name of the parameter to look up
	 * @param defaultValue the default value to return if the value was empty
	 * @return int the parsed double if successful, otherwise the defaultValue given
	 * @author Michael Sneddon
	 */
	double parseAsDouble(map<string,string> &argMap, string argName, double defaultValue);
}

#endif /* COMMANDLINEPARSER_HH_ */
