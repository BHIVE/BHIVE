/*
 * nfsimparascancomposer.hh
 *
 *  Created on: Jan 17, 2011
 *      Author: jentsch
 */

#ifndef NFSIMPARASCANCOMPOSER_HH_
#define NFSIMPARASCANCOMPOSER_HH_

#include "../../core/composer/composer.hh"
#include "../../core/communicator/serial/serialcommunicator.hh"
#include "../../core/input/commandLineParser/commandLineParser.hh"


// we will need to include the relevant header files from the nfsimparascanSimulator.
#include "nfsimAgentFactories/nfsimagentfactory.hh"
#include "nfsimAgentFactories/worldagentfactory.hh"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

using namespace std;
using namespace Hive;
using namespace NFs;


namespace nfsimparascan {
	// we start out with a serial implementation to get the
	// nfsim part working. after that, we will have to switch to
	// a parallel implementation. running this in parallel will
	// add a whole new level of complexity. ultimately, it would be
	// great if we could send molecules of NFsim through our parallel
	// pipeline.
	class NFsimParaScanComposer : public Composer {
	public:
		NFsimParaScanComposer();
		NFsimParaScanComposer(int argn, char *argv[]);
		~NFsimParaScanComposer();

		void setupSimulation();

	protected:
		void initializeAgentFactories();
		void setupAgentHierarchy();
		void addSerialCommunicator();
		// do we need something like this?
		void addParallelCommunicator();


		Agent* createAgent(AgentFactory& af);

		double output_interval;

		SerialCommunicator *post;

		map<string,string> commandline;

		string outputSuffix;

		bool binaryOutput;
	};

}

#endif /* NFSIMPARASCANCOMPOSER_HH_ */
