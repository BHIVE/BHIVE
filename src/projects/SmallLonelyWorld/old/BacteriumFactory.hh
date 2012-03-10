#ifndef BACTERIUMFACTORY_HH_
#define BACTERIUMFACTORY_HH_

#include "../../agent/agent.hh"
#include "../../agent/agentFactory/agent_factory.hh"
#include "../../exception/exception.hh"
using namespace Hive;


namespace SmallLonelyWorld
{
	class BacteriumFactory : public AgentFactory {
		public:
			BacteriumFactory();
			/// destructor
			~BacteriumFactory(){};
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

