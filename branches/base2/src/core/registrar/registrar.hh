/**
 * registrar.hh
 *
 *  Created on: Jun 18, 2010
 *      Author: msneddon
 */

#ifndef REGISTRAR_HH_
#define REGISTRAR_HH_


#include <map>
#include <vector>
#include <queue>

#include "../agent/agent.hh"
#include "../exception/exception.hh"



using namespace std;

namespace Hive {


	class Agent;
	class SpecialAgent;
	class AgentFactory;

	/** the official keeper of records
	 *
	 * This singleton class keeps all global records relating to the agents
	 * running on this processor, together with all global objects that may
	 * be needed, such as a global database, a list of global simulators, etc.
	 *
	 * This class also provides functionality to create and kill agents.  When
	 * the system Registrar is deleted, it takes down (deletes) all associated
	 * agents as well.
	 *
	 *
	 *
	 * @author msneddon
	 */
	class Registrar {

		public:

			/** Creates the Registrar for the system, which is global to this processor
			 * @return true if successful, false otherwise
			 */
			static bool createSystemRegistrar();


			/** Returns the active System Registrar
			 * @return the system's registrar
			 */
			static Registrar * getSystemRegistrar();


			/** Deletes the System Registrar and everything registered with it.
			 * @return true if successful, false otherwise
			 */
			static bool deleteSystemRegistrar();



			/** Retrieve the number of current registered agents
			 * Note: includes special agents
			 *
			 * @return number of active agents
			 */
			unsigned int getNumberOfAgents() const;


			/** Retrieve an agent with the given ID
			 *
			 * @param agentID of the agent to retrieve
			 * @return ptr to the agent
			 */
			Agent *getAgent(unsigned int agentID);



			/** Whenever an agent is born, you must register it
			 *
			 * For example, in the composer, you should do this:
			 * Agent *a = new Agent();
			 * Hive::Registrar::getSystemRegistrar()->registerNewAgent(a);
			 *
			 * @param newAgent the newly birthed agent
			 */
			void registerNewAgent(Agent *newAgent);



			/** Whenever an agent is born, you must register it with a special message
			 *
			 * For example, in the composer, you should do this:
			 * Agent *a = new Agent();
			 * Registrar::getSystemRegistrar()->registerNewAgent(a);
			 *
			 * This method will also add this child as a new child to the parent agent.
			 *
			 * @param newAgent the newly birthed agent
			 */
			void registerNewAgentAndSendMessageToSpecialAgent(Agent *newAgent, Message *m);



			/** Whenever a special agent is created, you must register it
			 *
			 * This should be handled by the composer by registering it first as
			 * an agent, in the same way as all other agents, then as a special
			 * agent.  For example:
			 *
			 * SpecialAgent *007 = new SpecialAgent();
			 * Registrar::getSystemRegistrar->registerNewAgent(007);
			 * Registrar::getSystemRegistrar->registerSpecialAgent(007);
			 *
			 *
			 * @param newAgent the newly birthed agent
			 */
			void registerNewSpecialAgent(SpecialAgent *newSpecialAgent);




			/** Use this function when creating agents to get an available ID
			 *
			 * @return available AgentID that was reserved
			 */
			unsigned int reserveNewAgentID();


			/** Use this function to ask the Registrar to do your dirty work for you
			 * This method also takes care of updating the parent so that it knows of
			 * the passing of this agent, and will also destroy all if the selected
			 * agent's children.
			 * @param agentID the ID of the unlucky agent
			 */
			void killAgent(unsigned int agentID);


			/** Use this function to ask the Registrar to do your dirty work for you
			 * This method does everything that killAgent does, with the added option
			 * to send a message of your choosing to notify the SpecialAgents, if there
			 * are any, of this event.  This is needed, for example, if the SpecialAgent
			 * must respond
			 * @param agentID the ID of the unlucky agent
			*/
			void killAgentAndSendMessageToSpecialAgent(unsigned int agentID, Message *m);



			/** Whenever an agent factory is created, you must register it
			 *
			 *
			 * @param agentFactory the agent factory
			 */
			void registerAgentFactory(AgentFactory *agentFactory);


			/** gets the number of registered agent factories
			 *
			 * @return number of agent factories registered
			 */
			int getNumberOfAgentFactories() const;


			/** retrieve an agent factory at index i.
			 *
			 * @param i the index of the registered agent factory
			 * @return the agent factory registered at index i
			 */
			AgentFactory * getAgentFactory(int i);





		protected:

			/** Registrar constructor
			 *  This is protected, so that only the static Register function createSystemRegistrar
			 *  can create a new one.  This also eliminates the need to pass around a copy of
			 *  the system registrar pointer, but allows you in the future to have multiple
			 *  registrars if need be.
			 */
			Registrar();
			/** Registrar deconstructor that also removes all registered agents */
			~Registrar();



			/** Agent map
			 * Maintains the list of agents indexed by their ID.  This will have to be
			 * updated once we go parallel to also identify agents by the processor
			 * from whence they came, or by maintaining a global unique agentID.
			 */
			map <int,Agent *> agentMap;
			map<int,Agent *>::iterator agentMapIter;


			/** Special Agent List
			 * Maintains the list of special agents.  Not that we enforce, for now, that
			 * there can ever only be one special agent in this list.  We eventually need
			 * to expand this, but it will be tricky, because how does a cell "know" which
			 * special agent to send the message to when it dies?
			 * @see Registrar::killAgentAndSendMessageToSpecialAgent
			 */
			vector <SpecialAgent *> specialAgentList;

			/** Agent Factory vector
			* Maintains the list of agent factories, so that agents can use them during
			* simulation to generate new agents as needed.
			*/
			vector <AgentFactory *> agentFactories;


		private:

			/** The single Registrar for the entire system */
			static Registrar * SystemRegistrar;

			/** AgentID counter */
			static unsigned int AgentIDCounter;

			/** queue to recycle agentIDs */
			static queue <unsigned int> AvailableAgentIDs;

	};

}



#endif /* REGISTRAR_HH_ */
