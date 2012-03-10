/*
 * worldagentfactoryI.hh
 *
 *  Created on: Apr 16, 2010
 *      Author: jentsch
 */

#ifndef WORLDAGENTFACTORYI_HH_
#define WORLDAGENTFACTORYI_HH_

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
#include "../chemopopSimulators/cellMovement/chemotaxisMovement/chemotaxisMovementInWorld.hh"
#include "../chemopopSimulators/cellMovement/blindSearch/blindMovementInWorld.hh"
#include "../chemopopSimulators/metabolism/simplestconsumptionsimulatorforworld.hh"
#include "../chemopopSimulators/environment/deluettenschieter.hh"
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
	 * @brief world agent factory number I of the chemotaxis population simulation.
	 *
	 *	wir muessen noch ein paar funktionen einfuegen ...
	 *
	 *
	 * @author Garrit Jentsch
	 *
	 * @date April 16th, 2010 last edited: 2010
	 */
	class WorldAgentFactoryI : public AgentFactory {
	public:
		WorldAgentFactoryI();
		~WorldAgentFactoryI();

		Agent* createAgent();
		Agent* createAgent(Action **as);

		Agent* duplicateAgent(Agent *ag);

		void finalise();

		int getNumSteps() { return num_steps; };
		double getTimeIncrement() { return dt; };

		void setInput(string);
		void setNumberCells(int i);
		void setOutputInterval(double oi) { this->output_interval = oi; };
		void setIsBlindAgentCells(bool isBlindAgent);
		void setBlockWorldOutput(bool blockWorldOutput) { this->blockWorldOutput = blockWorldOutput; };

	protected:
		void addActionsToAgentsActionSet(Agent *ag);
		void addSimulatorToAgent(Agent *ag);
		void addMessageGeneratorsToAgent(Agent *ag);

	private:
		int numbercells;
		int num_steps;
		double dt;
		double output_interval;

		bool isBlindAgent;
		bool blockWorldOutput;
		bool hasEnvironmentSimulator;

		string inputname;

		map<string,int> stringswitch; // this is needed for identifying the mathematical function
									  // describing the environment.
	};


}


#endif /* WORLDAGENTFACTORYI_HH_ */
