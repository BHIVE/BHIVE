/**
 * @file composer.hh
 *
 * this file contains the interface for the composer class.
 *
 * @author Garrit Jentsch
 *
 * @date 13th of Oct, 2009 last edited: Oct 14th, 2009 by Garrit and Michael
 *
 */

#ifndef COMPOSER_HH_
#define COMPOSER_HH_

#include <iostream>

#include "../agent/agent.hh"
#include "../agent/agentFactory/agent_factory.hh"
#include "../agent/special_agent.hh"

using namespace std;


namespace Hive
{

	/**
	 * @brief this class sets up the entire simulation.
	 *
	 * the composer sets up the entire simulation by coordinating all file reading,
	 * agent construction and agent initialisation. the setup of a simulation starts
	 * with the initialisation of the agentfactories. subsequently the agents are produced
	 * and the hierarchy among the agents is established.
	 *
	 * @todo Everything
	 *
	 * @author Garrit Jentsch
	 *
	 * @date 13th of Oct, 2009 last edited: 10-14-2009 by Garrit and Michael
	 *
	 */


	class Composer {
	public:
		/// constructor
		Composer() {
			maestro = NULL;
		};

		/// destructor
		virtual ~Composer() {
			// delete orchestra

			/*
			for (unsigned int i=0; i<orchestra.size(); i++) {
				delete orchestra[i];
				orchestra[i] = NULL;
			}
			// delete special agents
			for (unsigned int i=0; i<special_agents.size(); i++) {
				delete special_agents[i];
				special_agents[i] = NULL;
			}
			*/

			// delete factories
			for (unsigned int i=0; i<factories.size(); i++) {
				delete factories[i];
				factories[i] = NULL;
			}
		};

		/**
		 * returns pointer to special agent
		 * @return special_agent
		 */
		SpecialAgent* getSpecialAgent(int i) { return special_agents[i]; };

		/**
		 * returns pointer to the top level agent
		 * @return maestro
		 */
		Agent* getTopLevelAgent() { return maestro; };

		/**
		 *  top level method for constructing the simulation. for the beginning
		 *  we will re-implement this method for each new simulation
		 */
		virtual void setupSimulation()=0;

	protected:
		/**
		 *	prepare agent factories for agent setup
		 */
		virtual void initializeAgentFactories()=0;

		/**
		 * establishes child parent relationships among agents
		 */
		virtual void setupAgentHierarchy()=0;

		/**
		 * creates an agent
		 * @param af agentfactory that does the actual creation of agents.
		 * @return pointer to created agent
		 */
		virtual Agent* createAgent(AgentFactory& af)=0;

		/// top level agent
		Agent *maestro;

		/// vector of agents
		vector<Agent* > orchestra;

		/// vector of agentfactories for agent setup
		vector<AgentFactory* > factories;

		/// vector of special-agents
		vector<SpecialAgent* > special_agents;
	};
}
#endif /* COMPOSER_HH_ */
