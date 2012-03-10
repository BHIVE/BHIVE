/*
 *@file worldagentfactory.hh
 *
 *this file contains the interface to the world agent factory for the chemotaxis simulation
 *
 *@date Oct 20, 2009, last edited: Oct 20, 2009 by Garrit
 *
 *@author Garrit Jentsch
 */

#ifndef WORLDAGENTFACTORY_HH_
#define WORLDAGENTFACTORY_HH_


#include "../../agent/agentFactory/agent_factory.hh"
#include "diffusion_data.hh"
#include "../../input/meshinputreader/netgenmeshinputreader.hh"
#include "../../simulators/pde/PDESimulator.hh"
#include "../../agent/messageGenerator/chemotaxisMessageGenerator/chemotaxisMessageGenerator.hh"

#include "../../agent/data/grid/nvoxel.hh"
#include "../../agent/data/grid/grid.hh"

using namespace std;
using namespace Hive;

namespace Chemotaxis {

	/**
	 * @brief implementation of the worldagentfactory
	 *
	 * this is the worldagentfactory. it generates worldagent.
	 * a worldagent contains a grid on which several substances
	 * live.
	 *
	 * @author Garrit Jentsch
	 *
	 * @date Oct 14, 2009 last edited Oct 15, 2009
	 */
	class WorldAgentFactory : public AgentFactory {
	public:
		WorldAgentFactory();
		WorldAgentFactory(InputSystemReader *isr);

		Agent* createAgent();
		Agent* createAgent(Action** ac);
		Agent* duplicateAgent(Agent *ag);

		/**
		 * tell the agent factory which mesh input reader to use.
		 *
		 */
		void setMeshInputReader(NetgenMeshInputReader *mir);

		/**
		 * in this function we actually read the mesh and store all the grid information in
		 * a local copy.
		 */
		void finalise();

		void setNumberCells(int n) { this->number_cells = n; };
		int getNumberCells() { return number_cells; };

	protected:
		void addActionsToAgentsActionSet(Agent* a);
		void addSimulatorToAgent(Agent *a);
		void addMessageGenerators(Agent *ag);

	private:
		/// meshinputreader
		NetgenMeshInputReader *mir;
		/// local copy of the grid
		DiffData data;

		int number_cells;
	};
}

#endif /* WORLDAGENTFACTORY_HH_ */
