/*
 * blindagentfactory.hh
 *
 *  Created on: June 15th, 2010
 *      Author: jentsch, sneddon, colasanti
 */

#ifndef BLINDAGENTFACTORY_HH_
#define BLINDAGENTFACTORY_HH_

#include <cmath>

#include "../../agent/agentFactory/agent_factory.hh"
#include "../../agent/data/data.hh"
#include "../../agent/data/template/tdata.hh"
#include "../../agent/action/chemopopAction/chemopopAction.hh"
#include "../../agent/messageGenerator/chemopopMessageGenerator/chemopopMessageGenerator.hh"
#include "../../input/systemParser/inputsystemreader.hh"
#include "cellparameterinitializer.hh"
#include "../../simulators/cellMovement/blindSearch/blindAgentSimulator.hh"
#include "../../simulators/cellMovement/blindSearch/blindagentmovementsimulator.hh"
#include "../../simulators/cellMovement/blindSearch/exponentialrunlengthsimulator.hh"
#include "../../simulators/cellMovement/blindSearch/levyrunlengthsimulator.hh"
#include "../../simulators/metabolism/metabolismforblindagent.hh"
#include "../../simulators/deathandbirth/birthsimulator.hh"
#include "../../simulators/deathandbirth/deathsimulator.hh"
#include "../../simulators/timekeeper/timekeeper.hh"

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
	class BlindAgentFactory : public AgentFactory {
	public:
		BlindAgentFactory();
		BlindAgentFactory(InputSystemReader *isr);
		~BlindAgentFactory();

		void setInitializer(CellParameterInitializer *in) { this->cpi = in; };
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

#endif /* BLINDAGENTFACTORY_HH_ */
