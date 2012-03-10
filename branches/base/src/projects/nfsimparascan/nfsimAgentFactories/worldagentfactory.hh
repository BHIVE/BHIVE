/*
 * worldagentfactory.hh
 *
 *  Created on: Mar 15, 2011
 *      Author: jentsch
 */

#ifndef WORLDAGENTFACTORY_HH_
#define WORLDAGENTFACTORY_HH_

#include "../../../core/agent/agentFactory/agent_factory.hh"
#include "../../../core/agent/data/data.hh"
#include "../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../core/agent/data/template/tdata.hh"

#include "../../../core/exception/exception.hh"
#include "../../../core/simulators/timekeeper.hh"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;
using namespace Hive;

namespace nfsimparascan {

	class WorldAgentFactory : public AgentFactory {
	public:
		WorldAgentFactory();
		~WorldAgentFactory();

		Agent* createAgent();
		Agent* createAgent(Action **as);

		Agent* duplicateAgent(Agent *ag);

		void finalise();

	protected:
		void addActionsToAgentsActionSet(Agent *ag);
		void addSimulatorToAgent(Agent *ag);
		void addMessageGeneratorsToAgent(Agent *ag);

	private:

	};

}


#endif /* WORLDAGENTFACTORY_HH_ */
