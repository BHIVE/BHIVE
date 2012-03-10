/*
 * CellAgentFactory.hh
 *  Created on: 12-21-2010
 *      Author: chris and ric
 */

#ifndef CELLAGENTFACTORY_HH_
#define CELLAGENTFACTORY_HH_

#include "../../hive.hh"

using namespace std;
using namespace Hive;

namespace BatchReactor {
	class CellAgentFactory : public AgentFactory {
	public:
		CellAgentFactory();
		~CellAgentFactory();
		Agent* createAgent();
		Agent* createAgent(Action **as);
		Agent* duplicateAgent(Agent *ag);
		void finalise();
	protected:
		void addActionsToAgentsActionSet(Agent *ag);
		void addSimulatorToAgent(Agent *ag);
		void addMessageGeneratorsToAgent(Agent *ag);
	};
}

#endif /* CellAgentFactory_HH_ */
