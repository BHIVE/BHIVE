/*
 * nfAgentFactory.hh
 *
 *  Created on: Oct 20, 2009
 *      Author: msneddon
 */

#ifndef NFAGENTFACTORY_HH_
#define NFAGENTFACTORY_HH_


#include "../../agent.hh"
#include "../agent_factory.hh"
#include "../../data/nfdata/nfdata.hh"
#include "../../../simulators/nfsim/nf.hh"
#include "../../../simulators/nfsim/NFsim.hh"
#include "../../../exception/exception.hh"


namespace Hive
{

	/**
	 *  @brief agent factory for default NFsim agents
	 *
	 *  agent factory for constructing NFsim agents.
	 *
	 *  @todo Everything
	 *
	 *  @author Michael Sneddon
	 *
	 *  @date October 20, 2009   last edited: October 20, 2009
	 *
	 */

	class NF_AgentFactory : public AgentFactory {
		public:

			NF_AgentFactory(map<string,string> &argMap);

			/// destructor
			~NF_AgentFactory();

			/**
			 * create agent of a certain type. encapsulates everything that has to be done, i.e.
			 * it adds actions, simulator, messagegenerators, and communicator to the agent. the simulator is also told
			 * to connect to the database and initialize itself.
			 * @return pointer to constructed agent
			 */
			Agent* createAgent();

			void setupDatabase(Agent *a, NF::System *s);


			/**
			 * create an agent and provide actionset as an argument
			 * @param as pointer to actionset
			 */
			Agent* createAgent(Action ** as);

			/**
			 * copies an agent
			 * @param *ag pointer to agent to be copied
			 * @return pointer to copy
			 */
			Agent* duplicateAgent(Agent *ag);


			void finalise();


		protected:
			/**
			 * adds an action to the agents action set
			 * @param *a pointer to action
			 */
			void addActionToAgentsActionSet(Action *a);

			/**
			 * adds a simulator to the agents simulator set
			 * @param *s pointer to simulator
			 */
			void addSimulatorToAgent(Simulator *s, Agent *ag);


			/**
			 * adds an action to the agents action set
			 * @param *ag pointer to agent
			 */
			virtual void addActionsToAgentsActionSet(Agent *ag) {};

			/**
			 * adds a simulator to the agents simulator set
			 * @param *s pointer to simulator
			 */
			virtual void addSimulatorToAgent(Agent *ag) {};

		private:


			string xmlFilename;
			map<string,string> localArgMapCopy;
			bool verbose;



	};
}




#endif /* NFAGENTFACTORY_HH_ */
