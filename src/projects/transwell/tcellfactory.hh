/*
 * @file tcellfactory.hh
 *
 * file contains interface to tcell agent factory.
 *
 * @date December 15th, 2009 last edited 12-15-09
 *
 *@author Garrit Jentsch
 */

#ifndef TCELLFACTORY_HH_
#define TCELLFACTORY_HH_

#include "../../agent/agentFactory/agent_factory.hh"
#include "../../agent/data/rate/grate.hh"
#include "../../agent/action/transwellAction/transwellAction.hh"
#include "../../agent/messageGenerator/transwellMessageGenerator/transwellMessageGenerator.hh"
#include "../../input/systemParser/gSystemParser/gsystemreader.hh"
#include "../../input/system/gSystem/gsystem.hh"
#include "../../simulators/ode/grkode.hh"

using namespace std;
using namespace Hive;

namespace Transwell {
	/**
	 * @brief tcellagentfactory for producing tcell agent
	 * @todo we need a switch to determine whether a t_reg or a t_eff cell will be produced. we should also have some methods to set the input files.
	 */
	class TCellAgentFactory : public AgentFactory {
	public:
		TCellAgentFactory();
		TCellAgentFactory(InputSystemReader *isr);

		~TCellAgentFactory();

		Agent* createAgent();
		Agent* createAgent(Action **as);

		Agent* duplicateAgent(Agent *ag);

		// reads a gsystem using gsystemparser
		void finalise();

	protected:
		void addActionsToAgentsActionSet(Agent *ag);
		void addSimulatorToAgent(Agent *ag);
		void addMessageGeneratorsToAgent(Agent *ag);

	private:
		GSystem *gsys;
		GSystemReader *gsr;
	};
}

#endif /* TCELLFACTORY_HH_ */
