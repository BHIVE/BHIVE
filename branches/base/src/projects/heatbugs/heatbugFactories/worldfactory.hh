/*
 * worldfactory.hh
 *
 *  Created on: Dec 20, 2010
 *      Author: jentsch
 */

#ifndef WORLDFACTORY_HH_
#define WORLDFACTORY_HH_

#include <cmath>
#include <fstream>
#include <iostream>

#include "../../../core/agent/agentFactory/agent_factory.hh"
#include "../../../core/agent/data/data.hh"
#include "../../../core/agent/data/template/tdata.hh"
#include "../../../core/simulators/timekeeper/timekeeper.hh"
#include "../../../core/util/util.hh"

#include "../heatbugAction/heatbugAction.hh"
#include "../heatbugMessageGenerator/heatbugMessageGenerator.hh"
#include "../heatbugSimulators/diffusionsimulator.hh"
#include "../heatbugSimulators/simulatorforresolvingconflicts.hh"

using namespace std;
using namespace Hive;

namespace Heatbug {

	/** @brief WorldFactory: Agent Factory for the world in the heatbug simulation.
	 *
	 *	@author garrit jentsch
	 *
	 *	@date December 20th, 2010
	 *
	 */
	class WorldFactory : public AgentFactory {
		public:
			WorldFactory();
			~WorldFactory();

			void setOutputInterval(double oi) { this->output_interval = oi; };

			/// new method for setting up the number of bugs that will live in the world.
			void setNumberBugs(int i) { this->number_bugs = i;};

			void setGridSize(int i) { this->gridsize = i;};

			Agent* createAgent();

			Agent* createAgent(Action **as);

			Agent* duplicateAgent(Agent *ag);

			void finalise();

		protected:
			void addActionsToAgentsActionSet(Agent *ag);

			void addSimulatorToAgent(Agent *ag);

			void addMessageGeneratorsToAgent(Agent *ag);

			double output_interval;

			int number_bugs;

			int gridsize;
	};

}


#endif /* WORLDFACTORY_HH_ */
