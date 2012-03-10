/*
 * BatchReactor_HH_
 *  Created on: Feb 7, 2010
 *      Author: chenry and ric
 */

#ifndef BATCHWORLD_HH_
#define BATCHWORLD_HH_

#include "../../hive.hh"

using namespace std;
using namespace Hive;

namespace BatchReactor {
	class BatchWorldFactory : public AgentFactory {
	public:
		BatchWorldFactory();
		~BatchWorldFactory();
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

#endif /* BatchReactor_HH_ */
