/*
 * heatbugcomposer.hh
 *
 *  Created on: Jan 4, 2011
 *      Author: jentsch
 */

#ifndef HEATBUGCOMPOSER_HH_
#define HEATBUGCOMPOSER_HH_

#include "../../core/composer/composer.hh"
#include "../../core/communicator/serial/serialcommunicator.hh"
#include "../../core/input/commandLineParser/commandLineParser.hh"

#include "heatbugFactories/worldfactory.hh"
#include "heatbugFactories/bugfactory.hh"
#include "heatbugFactories/outputagentfactory.hh"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

using namespace std;
using namespace Hive;

namespace Heatbug {

	class HeatbugComposer : public Composer {
	public:
		HeatbugComposer();
		HeatbugComposer(int argn, char *argv[]);
		~HeatbugComposer();
		int getNumSteps();
		double getTimeIncrement();
		void setNumberOfBugs(int);
		void setupSimulation();

	protected:
		void initializeAgentFactories();
		void setupAgentHierarchy();
		void addSerialCommunicator();

		Agent* createAgent(AgentFactory& af);

		int number_bugs;
		int num_steps;

		double dt;
		double output_interval;

		SerialCommunicator *post;

		map<string,string> commandline;

		string outputSuffix;

		bool binaryOutput;
	};

}

#endif /* HEATBUGCOMPOSER_HH_ */
