#ifndef WORLDFACTORY_HH_
#define WORLDFACTORY_HH_

#include "../../agent/agent.hh"
#include "../../agent/agentFactory/agent_factory.hh"
#include "../../exception/exception.hh"
using namespace Hive;


namespace SmallLonelyWorld
{
	class WorldFactory : public AgentFactory {
		public:
			WorldFactory();
			/// destructor
			~WorldFactory(){};
			Agent* createAgent();
			void setupDatabase(Agent *a, int boxnum, int foodmax) {};

			Agent* createAgent(Action ** as){return NULL;};
			Agent* duplicateAgent(Agent *ag){return NULL;};
			void finalise(){};

		protected:
			void addActionsToAgentsActionSet(Agent *ag) {};
			void addSimulatorToAgent(Agent *ag) {};
	};
}

#endif /* WORLDFACTORY_HH_ */

