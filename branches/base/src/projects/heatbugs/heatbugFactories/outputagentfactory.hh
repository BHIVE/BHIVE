/*
 * outputagentfactory.hh
 *
 *  Created on: Jan 5, 2011
 *      Author: jentsch
 */

#ifndef HB_OUTPUTAGENTFACTORY_HH_
#define HB_OUTPUTAGENTFACTORY_HH_

#include "../../../core/agent/agentFactory/agent_factory.hh"
#include "../../../core/agent/special_agent.hh"
#include "../../../core/agent/data/data.hh"
#include "../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../core/agent/data/template/tdata.hh"
#include "../heatbugAction/heatbugAction.hh"

using namespace std;
using namespace Hive;

/** @brief class for setting up an output agent
 *
 * @author garrit
 *
 * @date 1-5-2011
 */

namespace Heatbug {

class OutputAgentFactory : public AgentFactory {
public:
	OutputAgentFactory();
	~OutputAgentFactory();

	Agent* createAgent();
	Agent* createAgent(Action **as);

	Agent* duplicateAgent(Agent *ag);

	void finalise();

	void setNumberBugs(int n) { this->number_bugs = n; };

	void setOutputInterval(double d) { this->output_interval = d;};

	void setOutputStyle(bool b) { this->binaryOutput=b; };

	void setOutputFileNameSuffix(string s) {this->outputSuffix=s;};

protected:
	void addActionsToAgentsActionSet(Agent *ag);
	void addSimulatorToAgent(Agent *ag);
	void addMessageGeneratorsToAgent(Agent *ag);

private:
	int    number_bugs;
	double output_interval;
	bool   binaryOutput;
	string outputSuffix;
};

}


#endif /* HB_OUTPUTAGENTFACTORY_HH_ */
