/*
 * blindagentfactory.hh
 *
 *  Created on: June 15th, 2010
 *      Author: jentsch, sneddon, colasanti
 */

#ifndef BLINDAGENTFACTORY_HH_
#define BLINDAGENTFACTORY_HH_

#include <cmath>

#include "../../../core/agent/agentFactory/agent_factory.hh"
#include "../../../core/agent/data/data.hh"
#include "../../../core/agent/data/template/tdata.hh"
#include "../../../core/simulators/timekeeper/timekeeper.hh"

#include "../chemopopAction/chemopopAction.hh"
#include "../chemopopMessageGenerator/chemopopMessageGenerator.hh"
#include "../chemopopInitialiser/cellparameterinitializer.hh"
#include "../chemopopSimulators/cellMovement/blindSearch/blindAgentSimulator.hh"
#include "../chemopopSimulators/cellMovement/blindSearch/blindagentmovementsimulator.hh"
#include "../chemopopSimulators/cellMovement/blindSearch/exponentialrunlengthsimulator.hh"
#include "../chemopopSimulators/cellMovement/blindSearch/levyrunlengthsimulator.hh"
#include "../chemopopSimulators/metabolism/metabolismforblindagent.hh"
#include "../chemopopSimulators/deathandbirth/birthsimulator.hh"
#include "../chemopopSimulators/deathandbirth/deathsimulator.hh"


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
