/*
 * outputagentfactory.hh
 *
 *  Created on: Apr 16, 2010
 *      Author: jentsch
 */

#ifndef OUTPUTAGENTFACTORY_HH_
#define OUTPUTAGENTFACTORY_HH_


#include "../../../core/agent/agentFactory/agent_factory.hh"
#include "../../../core/agent/special_agent.hh"
#include "../../../core/agent/data/data.hh"
#include "../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../core/agent/data/template/tdata.hh"
#include "../chemopopAction/chemopopAction.hh"

using namespace std;
using namespace Hive;

/** @brief class for setting up an output agent
 *
 * @author garrit
 *
 * @date 4-16-2010
 */

namespace ChemoPop {
	class OutputAgentFactory : public AgentFactory {
	public:
		OutputAgentFactory();
		~OutputAgentFactory();

		Agent* createAgent();
		Agent* createAgent(Action **as);

		Agent* duplicateAgent(Agent *ag);

		void finalise();

		void setNumberBlindAgents(int);

		void setNumberCells(int);

		void setOutputInterval(double d) { this->output_interval = d;};

		void setWorldHasGrid(bool b) { this->world_has_grid = b;};

		void setOutputStyle(bool b) { this->binaryOutput=b; };
		void setOutputFileNameSuffix(string s) {this->outputSuffix=s;};

	protected:
		void addActionsToAgentsActionSet(Agent *ag);
		void addSimulatorToAgent(Agent *ag);
		void addMessageGeneratorsToAgent(Agent *ag);

	private:
		int numbercells;
		int numberblindagents;
		double output_interval;

		bool binaryOutput;
		bool world_has_grid;
		string outputSuffix;
	};

}

#endif /* OUTPUTAGENTFACTORY_HH_ */
