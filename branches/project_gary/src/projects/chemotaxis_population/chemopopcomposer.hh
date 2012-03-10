/**
 *  @file chemopopcomposer.hh
 *
 *  @brief  interface to the
 *
 * Created on: April 16, 2010
 * author : garrit
 */

#ifndef CHEMOPOPCOMPOSER_HH_
#define CHEMOPOPCOMPOSER_HH_

#include "../../composer/composer.hh"
#include "../../communicator/serial/serialcommunicator.hh"
#include "../../input/commandLineParser/commandLineParser.hh"

#include "worldagentfactoryI.hh"
#include "agentinitializer.hh"
#include "cellparameterinitializer.hh"
#include "blindagentinitializer.hh"
#include "cellagentfactory.hh"
#include "blindagentfactory.hh"
#include "outputagentfactory.hh"

#include <iostream>
#include <map>
#include <string>

using namespace std;
using namespace Hive;

namespace ChemoPop {
	/**
	 * @brief composer for setting up the chemotaxis population simulations
	 *
	 * @author Garrit
	 *
	 * @date April 16, 2010 last edited: 4-16-2010 by garrit
	 */

	class ChemoPopComposer : public Composer {
	public:
		ChemoPopComposer();
		ChemoPopComposer(int argn, char *argv[]);

		~ChemoPopComposer();

		int getNumSteps() { return num_steps; };

		double getTimeIncrement() { return dt; };

		void setNumberOfCells(int);

		void setupSimulation();

	protected:
		void initializeAgentFactories();

		void setupAgentHierarchy();

		void addSerialCommunicator();

		Agent* createAgent(AgentFactory& af);

		int number_of_cells;
		int num_steps;

		double dt;
		double output_interval;

		SerialCommunicator *post;

		map<string,string> commandline;

		AgentInitializer *cpi;

		string outputSuffix;
		bool binaryOutput;

		bool cells_are_blind_agents;

		bool blockWorldOutput;
	};
}


#endif // CHEMOPOPCOMPOSER_HH_
