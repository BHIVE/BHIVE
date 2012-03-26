/*
 * worldagentfactoryII.hh
 *
 *  Created on: Oct 1st, 2010
 *      Author: jentsch
 */

#ifndef WORLDAGENTFACTORYII_HH_
#define WORLDAGENTFACTORYII_HH_

#include "../../../core/agent/agentFactory/agent_factory.hh"
#include "../../../core/agent/data/data.hh"
#include "../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../core/agent/data/mathfunction/mathfunctiondata.hh"
#include "../../../core/agent/data/template/tdata.hh"
#include "../../../core/agent/data/mapdata/mapintintdata.hh"
#include "../../../core/exception/exception.hh"
#include "../../../core/simulators/timekeeper/timekeeper.hh"

#include "../chemopopAction/chemopopAction.hh"
#include "../chemopopData/grid/grid.hh"
#include "../chemopopMessageGenerator/chemopopMessageGenerator.hh"
#include "../chemopopInput/gwaparser.hh"

#include "../chemopopSimulators/cellMovement/chemotaxisMovement/chemotaxisMovementInWorld.hh"
#include "../chemopopSimulators/cellMovement/blindSearch/blindMovementInWorld.hh"
#include "../chemopopSimulators/metabolism/simplestconsumptionsimulatorforworld.hh"
#include "../chemopopSimulators/environment/deluettenschieter.hh"
#include "../chemopopSimulators/pde/cranknicolson.hh"

#include "../chemopopInitialiser/cubeconstantinitialiser.hh"
#include "../chemopopInitialiser/cuberandominitialiser.hh"
#include "../chemopopInitialiser/cellpositioninitialiser.hh"

#include "../cubegeometryconstructor.hh"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;
using namespace Hive;

namespace ChemoPop {

	/**
	 * @brief world agent factory number II of the chemotaxis population simulation.
	 *
	 *	this one uses a parser to read the system.
	 *
	 *
	 * @author Garrit Jentsch
	 *
	 * @date Oct 1st, 2010 last edited: 2010
	 */
	class WorldAgentFactoryII : public AgentFactory {
	public:
		WorldAgentFactoryII();
		~WorldAgentFactoryII();

		Agent* createAgent();
		Agent* createAgent(Action **as);

		Agent* duplicateAgent(Agent *ag);

		void finalise();

		int getNumCells() { return numbercells; };
		int getNumSteps() { return num_steps; };
		double getTimeIncrement() { return dt; };
		double getEquilibrationTime() { return equilibration_time; };

		void setInput(string);
		void setNumberCells(int i);
		void setOutputInterval(double oi) { this->output_interval = oi; };
		void setOutputSuffix(string suffix) { this->outputsuffix = suffix; };
		void setIsBlindAgentCells(bool isBlindAgent);
		void setBlockWorldOutput(bool blockWorldOutput) { this->blockWorldOutput = blockWorldOutput; };

		void setGenerateCellPositionOutput(bool b) { this->generate_cell_position_output = b ;};
		void setGenerateRunTumbleOutput(bool b) { this->generate_run_tumble_output = b; };

		void setTumbleTriggeredAverageParameters(
				    bool b,
				    double offset,
					double start,
					double end) {
			this->generate_traj_before_tumble_output = b;
			this->generateTrajBeforeTumbleTimeOFFSET=offset;
			this->generateTrajBeforeTumbleTimeSTART=start;
			this->generateTrajBeforeTumbleTimeEND=end;
		};

	protected:
		void addActionsToAgentsActionSet(Agent *ag);
		void addSimulatorToAgent(Agent *ag);
		void addMessageGeneratorsToAgent(Agent *ag);

	private:
		int numbercells;
		int num_steps;
		double dt;
		double output_interval;
		double equilibration_time;

		bool isBlindAgent;
		bool blockWorldOutput;
		bool hasEnvironmentSimulator;

		// flags for output
		bool generate_cell_position_output;
		bool generate_run_tumble_output;

		// parameters for tumble triggered averages
		bool generate_traj_before_tumble_output;
		double generateTrajBeforeTumbleTimeOFFSET;
		double generateTrajBeforeTumbleTimeSTART;
		double generateTrajBeforeTumbleTimeEND;

		string inputname;
		string outputsuffix;

		map<string,int> stringswitch; // this is needed for identifying the mathematical function
									  // describing the environment.

	};


}


#endif /* WORLDAGENTFACTORYI_HH_ */
