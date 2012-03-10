/*
 * bugfactory.hh
 *
 *  Created on: Dec 20, 2010
 *      Author: jentsch
 */

#ifndef BUGFACTORY_HH_
#define BUGFACTORY_HH_

#include <cmath>
#include <fstream>
#include <iostream>

#include "../../../core/agent/agentFactory/agent_factory.hh"
#include "../../../core/agent/data/data.hh"
#include "../../../core/agent/data/template/tdata.hh"
#include "../../../core/simulators/timekeeper/timekeeper.hh"

#include "../heatbugAction/heatbugAction.hh"
#include "../heatbugMessageGenerator/heatbugMessageGenerator.hh"
#include "../heatbugSimulators/movementdecisionsimulator.hh"


using namespace std;
using namespace Hive;

namespace Heatbug {

	/** @brief BugFactory: Agent Factory for the bugs in the heatbug simulation.
	 *
	 *	@author garrit jentsch
	 *
	 *	@date December 20th, 2010
	 *
	 */
	class BugFactory : public AgentFactory {
		public:
			BugFactory();
			~BugFactory();

			void setOutputInterval(double oi) { this->output_interval = oi; };

			void setProgram(string program){this->program = program;}

			Agent* createAgent();

			Agent* createAgent(Action **as);

			Agent* duplicateAgent(Agent *ag);

			void finalise();

		protected:
			void addActionsToAgentsActionSet(Agent *ag);

			void addSimulatorToAgent(Agent *ag);

			void addMessageGeneratorsToAgent(Agent *ag);

			double output_interval;

			string program;
	};

}


#endif /* BUGFACTORY_HH_ */
