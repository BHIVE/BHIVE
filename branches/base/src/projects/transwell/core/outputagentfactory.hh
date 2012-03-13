/*
 *@file outputagentfactory.hh
 *
 *this file contains the  ...
 *
 *@date Jan 19, 2010
 *@author garrit jentsch
 */

#ifndef OUTPUTAGENTFACTORY_HH_
#define OUTPUTAGENTFACTORY_HH_

#include "../../../core/agent/agentFactory/agent_factory.hh"
#include "../../../core/agent/special_agent.hh"
#include "../../../core/agent/data/primitive/primitiveData.hh"
#include "../transwellAction/transwellAction.hh"

using namespace std;
using namespace Hive;

/**
 * @brief class for setting up an output agent.
 *
 * @todo
 *
 * @author Garrit Jentsch
 * @date Jan 19th, 2010
 */

namespace Transwell {
	class OutputAgentFactory : public AgentFactory {
	public:
		OutputAgentFactory();

		~OutputAgentFactory();

		Agent* createAgent();
		Agent* createAgent(Action **as);

		Agent* duplicateAgent(Agent *ag);

		void finalise();

		void setNumberCells(int);

		void setNumberTeffsTregs(int teff, int treg);

	protected:
		void addActionsToAgentsActionSet(Agent *ag);
		void addSimulatorToAgent(Agent *ag);
		void addMessageGeneratorsToAgent(Agent *ag);

	private:
		int numbercells;
		int number_tregs;
		int number_teffs;

		bool two_cell_types;
	};
}



#endif /* OUTPUTAGENTFACTORY_HH_ */
