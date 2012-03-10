/**
 * @file dataParser.hh
 *
 * Functions to parse a file and automatically create data objects to insert to
 * an existing agent database.
 *
 * @date Dec 2011
 *
 * @author Michael Sneddon
 */


#ifndef DATAPARSER_HH_
#define DATAPARSER_HH_


#include "../../util/util.hh"
#include "../../agent/agent.hh"
#include "../../agent/data/data.hh"
#include "../../agent/data/primitive/primitiveData.hh"

#include <map>
#include <string>

using namespace std;

namespace Hive
{



	class DataParser {

		public:
			DataParser() {};
			virtual ~DataParser() {};


			//! Parses the file into Data objects .
			/*!
			 * Given the vector of strings taken from the command line, this function
			 * parses out all strings that start with a dash and identifies them as
			 * parameters, and attatches the parameter value to whatever string follows.
			 * For example, -file help.txt would add an entry to the argMap as a
			 * parameter named "file" with value "help.txt".  You can take a look
			 * at parseAsInt and parseAsDouble functions that can interpret the value
			 * as integers or doubles
			 *
			 * @param filename the filename (including path) of the file to parse
			 * @param argv the array of character arrays (strings)
			 * @param argMap the map that will be set when this function is called
			 * @author Michael Sneddon
			 */
			virtual void parse(string filename, Agent *a, bool verbose);

		protected:
			virtual void parseSingleLine(const string line, string &data_type, string &data_name, string &data_value, const string line_number, const string filename);

			// may want to add a batch parse function that takes one file, and adds the same
			// items to many agents.  This will be much faster for some simulations, but is
			// not worth my time now.  --michael

	};


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
