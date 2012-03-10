/*
 * agentparser.hh
 *
 * basic class from which to derive the parsers that will be employed by agent-factories.
 * when deriving from this class one will have to overwrite the protected method doIt!
 * it should be straightforward to derive any kind of parser such as an xml parser from this file.
 *
 *
 *  Created on: Sep 28, 2010
 *      Author: jentsch
 *
 */

#ifndef AGENTPARSER_HH_
#define AGENTPARSER_HH_

// including some basic std classes for parsing txt input
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// include agent class
#include "../../agent/agent.hh"
#include "../../exception/exception.hh"

using namespace std;

namespace Hive {

	class AgentParser {
	public:
		AgentParser();
		virtual ~AgentParser();

		// multiple ways to call the parser
		void parse(Agent*, ifstream&);

		void parse(Agent*, string);

		void parse(Agent*);

		void parse();

		// setting up the locale variables
		void setAgent(Agent*);

		void setInputfile(ifstream&);

		void setInputfile(string);

	protected:
		virtual void doIt();

		// we keep a pointer to the agent
		Agent *ag;

		// input file stream
		ifstream* input;

		// some flag
		bool input_has_been_assigned;
	};
}


#endif /* AGENTPARSER_HH_ */
