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

#include "../../core/composer/composer.hh"
#include "../../core/communicator/serial/serialcommunicator.hh"
#include "../../core/input/commandLineParser/commandLineParser.hh"

#include "chemopopFactories/blindagentfactory.hh"
#include "chemopopFactories/cellagentfactory.hh"
#include "chemopopFactories/outputagentfactory.hh"
#include "chemopopFactories/worldagentfactoryI.hh"
#include "chemopopFactories/worldagentfactoryII.hh"

#include "chemopopInitialiser/agentinitializer.hh"
#include "chemopopInitialiser/blindagentinitializer.hh"
#include "chemopopInitialiser/cellparameterinitializer.hh"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
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

		int getNumSteps();

		double getTimeIncrement();

		void setNumberOfCells(int);

		void setupSimulation();

		// output values at the end
		void outputTrajectoriesBeforeTumblesIfRequested();


	protected:
		void determineCellType(ifstream& input, string& filename_for_initialiser);

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

		/// some flags
		bool binaryOutput;               // determines output mode
		bool cells_are_blind_agents;     // type of simulation
		bool blockWorldOutput;           // suppresses output of world information!

		/// some more flags for yann ... should be more or less selfexplanatory
		bool generate_cell_data_output;
		bool generate_cell_position_output;
		bool generate_run_tumble_output;
		bool generate_traj_before_tumble_output;

	};
}


#endif // CHEMOPOPCOMPOSER_HH_
