/**
 *@file  dummyagentfactory.hh
 *
 *this file contains the interface to the dummyagentfactory class
 *
 *@author Garrit Jentsch
 *
 *@date  Oct 14th, 2009 last edited Oct 15th, 2009 by Garrit
 */

#ifndef DUMMYAGENTFACTORY_HH_
#define DUMMYAGENTFACTORY_HH_

#include "../../agent/agentFactory/agent_factory.hh"

using namespace std;
using namespace Hive;

namespace Dummy
{

	/**
	 * @brief simple implementation of an agentfactory
	 *
	 * this is a minimal implemtation of the agentfactory. it generates
	 * agents that can not really do anything.
	 *
	 * @author Garrit Jentsch
	 *
	 * @date Oct 14, 2009 last edited Oct 15, 2009
	 */

	class DummyAgentFactory : public AgentFactory {
	public:
		DummyAgentFactory();
		DummyAgentFactory(InputSystemReader *isr);

		Agent* createAgent();
		Agent* createAgent(Action** ac);
		Agent* duplicateAgent(Agent *ag);

		void finalise();

	protected:
		void addActionsToAgentsActionSet(Agent* a);
		void addSimulatorToAgent(Agent *ag);


	};
}
#endif /* DUMMYAGENTFACTORY_HH_ */
