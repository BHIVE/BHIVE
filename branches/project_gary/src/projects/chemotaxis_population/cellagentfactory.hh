/*
 * cellagentfactory.hh
 *
 *  Created on: Apr 16, 2010
 *      Author: jentsch
 */

#ifndef CELLAGENTFACTORY_HH_
#define CELLAGENTFACTORY_HH_


#include "../../agent/agentFactory/agent_factory.hh"
#include "../../agent/data/data.hh"
#include "../../agent/data/template/tdata.hh"
#include "../../agent/action/chemopopAction/chemopopAction.hh"
#include "../../agent/messageGenerator/chemopopMessageGenerator/chemopopMessageGenerator.hh"
#include "../../input/systemParser/inputsystemreader.hh"
#include "../../simulators/metabolism/simplestcellmetabolismsimulator.hh"
#include "../../simulators/timekeeper/timekeeper.hh"
#include "cellparameterinitializer.hh"
#include "cellSimulators/cellSimulators.hh"

using namespace std;
using namespace Hive;

namespace ChemoPop {
	/**
	 * @brief cell agent factory
	 *
	 * @author garrit
	 *
	 * @date April 16, 2009, last edited 4-16-10 by g.
	 */
	class CellAgentFactory : public AgentFactory {
	public:
		CellAgentFactory();
		CellAgentFactory(InputSystemReader *isr);
		~CellAgentFactory();

		void setInitializer(CellParameterInitializer *in) { this->cpi = in; };
		// this is for setting output interval
		void setOutputInterval(double oi) { this->output_interval = oi; };

		Agent* createAgent();
		Agent* createAgent(Action **as);

		Agent* duplicateAgent(Agent *ag);

		void finalise();

	protected:
		void addActionsToAgentsActionSet(Agent *ag);
		void addSimulatorToAgent(Agent *ag);
		void addMessageGeneratorsToAgent(Agent *ag);


		string inputFileName;

		CellParameterInitializer * cpi;

		double output_interval;
	private:

	};
}

#endif /* CELLAGENTFACTORY_HH_ */
