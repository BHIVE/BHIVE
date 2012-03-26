/*
 * @file nucleusagentfactory.hh
 *
 * file contains interface to nucleus agent factory
 *
 * @date January 12th, 2010 last edited 01-12-10
 *
 * @author Garrit Jentsch
 */

#ifndef NUCLEUSAGENT_HH_
#define NUCLEUSAGENT_HH_

#include "../../../core/agent/agentFactory/agent_factory.hh"
#include "../input/systemParser/gSystemParser/gsystemreader.hh"
#include "../input/system/gSystem/gsystem.hh"
#include "../simulators/ssa/direct_ssa.hh"
#include "../../../core/agent/data/primitive/primitiveData.hh"
#include "../transwellAction/transwellAction.hh"
#include "../transwellMessageGenerator/transwellMessageGenerator.hh"

using namespace std;
using namespace Hive;

namespace Transwell {
	class NucleusAgentFactory : public AgentFactory {
	public:
		NucleusAgentFactory();

		~NucleusAgentFactory();

		Agent* createAgent(bool for_treg_cell);
	       
                Agent* createAgent();

		Agent* createAgent(Action **as);

		Agent* duplicateAgent(Agent *ag);

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


#endif /* NUCLEUSAGENT_HH_ */
