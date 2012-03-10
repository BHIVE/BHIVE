/**
 * @file chemotaxiscomposer.hh
 *
 *this file contains the interface to the composer of the chemotaxis simulation.
 *
 *@date Oct 20th, 2009, last edited 10-20-2009 by garrit
 *@author garrit jentsch
 */

#include "../../composer/composer.hh"
#include "worldagentfactory.hh"
#include "../../agent/agentFactory/chemotaxisCellFactory/chemotaxisCellFactory.hh"
#include "../../communicator/serial/serialcommunicator.hh"
#include "../../agent/agentFactory/nfAgentFactory/nfAgentFactory.hh"
#include "chemotaxisnfsimactions.hh"

#include <iostream>

#ifndef CHEMOTAXISCOMPOSER_HH_
#define CHEMOTAXISCOMPOSER_HH_

using namespace std;
using namespace Hive;

/**
 * @brief composer for setting up the chemotaxis simulation
 *
 * this class coordinates the set up of the entire chemotaxis simulation.
 *
 * @author  Garrit Jentsch
 *
 * @date Oct, 20th 2009, last edited 10-20-2009 by garrit
 */

namespace Chemotaxis {
	class ChemotaxisComposer : public Composer {
	public:
		ChemotaxisComposer();
		~ChemotaxisComposer();
		virtual void setupSimulation();

		/// generates output for visualizing the bugs
		void generateBugOutput(ofstream& );

		/// generates output for visualising the diffusing chemicals
		void generateChemicalOutput(ofstream&);

	protected:
		/**
		 *	prepare agent factories for agent setup
		 */
		virtual void initializeAgentFactories();

		/**
		 * establishes child parent relationships among agents
		 */
		virtual void setupAgentHierarchy();

		void addSerialCommunicator();

		/**
		 * creates an agent
		 * @param af agentfactory that does the actual creation of agents.
		 * @return pointer to created agent
		 */
		virtual Agent* createAgent(AgentFactory& af);
	};

}
#endif /* CHEMOTAXISCOMPOSER_HH_ */
