/*
 * worldagentfactoryI.hh
 *
 *  Created on: Apr 16, 2010
 *      Author: jentsch
 */

#ifndef WORLDAGENTFACTORYI_HH_
#define WORLDAGENTFACTORYI_HH_

#include "../../agent/agentFactory/agent_factory.hh"
#include "../../agent/data/data.hh"
#include "../../agent/data/primitive/primitiveData.hh"
#include "../../agent/data/mathfunction/mathfunctiondata.hh"
#include "../../agent/data/template/tdata.hh"
#include "../../agent/data/grid/grid.hh"
#include "../../agent/data/mapdata/mapintintdata.hh"
#include "../../agent/action/chemopopAction/chemopopAction.hh"
#include "../../agent/messageGenerator/chemopopMessageGenerator/chemopopMessageGenerator.hh"
#include "../../input/systemParser/inputsystemreader.hh"
#include "../../simulators/cellMovement/chemotaxisMovement/chemotaxisMovementInWorld.hh"
#include "../../simulators/cellMovement/blindSearch/blindMovementInWorld.hh"
#include "../../simulators/metabolism/simplestconsumptionsimulatorforworld.hh"
#include "../../simulators/timekeeper/timekeeper.hh"
#include "../../simulators/environment/deluettenschieter.hh"
#include "cubeconstantinitialiser.hh"
#include "cuberandominitialiser.hh"

#include "cellpositioninitialiser.hh"
#include "cubegeometryconstructor.hh"

#include "../../exception/exception.hh"


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
		WorldAgentFactoryI(InputSystemReader *isr);
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
