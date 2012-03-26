/*
 * cellagentfactory.hh
 *
 *  Created on: Apr 16, 2010
 *      Author: jentsch
 */

#ifndef CELLAGENTFACTORY_HH_
#define CELLAGENTFACTORY_HH_


#include "../../../core/agent/agentFactory/agent_factory.hh"
#include "../../../core/agent/data/data.hh"
#include "../../../core/agent/data/template/tdata.hh"
#include "../../../core/simulators/timekeeper/timekeeper.hh"
#include "../chemopopData/historyCollector.hh"

#include "../chemopopAction/chemopopAction.hh"
#include "../chemopopMessageGenerator/chemopopMessageGenerator.hh"
#include "../chemopopSimulators/metabolism/simplestcellmetabolismsimulator.hh"
#include "../chemopopInitialiser/cellparameterinitializer.hh"
#include "../chemopopSimulators/cellSimulators/cellSimulators.hh"

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
		~CellAgentFactory();

		void setInitializer(CellParameterInitializer *in) { this->cpi = in; };
		// this is for setting output interval
		void setOutputInterval(double oi) { this->output_interval = oi; };

		Agent* createAgent();
		Agent* createAgent(Action **as);

		Agent* duplicateAgent(Agent *ag);

		void finalise();

		void setEquilibrationTime(double d) { this->equilibration_time = d; };
		void setGenerateCellDataOutput(bool b) { this->generate_cell_data_output = b; };

		void setTumbleTriggeredAverageParameters(
				    bool b,
				    double offset,
					double start,
					double end) {
			this->generate_traj_before_tumble_output = b;
			this->generateTrajBeforeTumbleTimeOFFSET=offset;
			this->generateTrajBeforeTumbleTimeSTART=start;
			this->generateTrajBeforeTumbleTimeEND=end;
		};

	protected:
		void addActionsToAgentsActionSet(Agent *ag);
		void addSimulatorToAgent(Agent *ag);
		void addMessageGeneratorsToAgent(Agent *ag);


		string inputFileName;

		CellParameterInitializer * cpi;

		double output_interval;

		bool generate_cell_data_output;

		double equilibration_time;


		// parameters for tumble triggered averages
		bool generate_traj_before_tumble_output;
		double generateTrajBeforeTumbleTimeOFFSET;
		double generateTrajBeforeTumbleTimeSTART;
		double generateTrajBeforeTumbleTimeEND;



	private:

	};
}

#endif /* CELLAGENTFACTORY_HH_ */
