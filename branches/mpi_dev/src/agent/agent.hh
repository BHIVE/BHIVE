/**
 * @file agent.hh
 *
 *	this file contains the interface to the agent class
 *
 * @date Oct 12th, 2009   last edited: Oct 15th, 2009 by Garrit and Michael
 *
 * @author Garrit Jentsch
 */

#ifndef AGENT_HH_
#define AGENT_HH_

#include <iostream>
#include <vector>
#include <queue>

#include "action/arnie.hh"
#include "data/data.hh"
#include "database/database.hh"
#include "message/message.hh"
#include "messageGenerator/messagegenerator.hh"

#include "../communicator/communicator.hh"
#include "../output/outputwriter.hh"
#include "../simulators/simulator.hh"

using namespace std;


namespace Hive
{
	// forward declarations for cyclic dependencies
	class Action;
	class Message;
	class MessageGenerator;
	class Communicator;
	class Simulator;



	/**
	 * @brief central class of the hive
	 *
	 * the agent is the central class of the hive. an agent contains a database in which it stores
	 * its knowledge. its simulators operate on the data stored within the database. communication among
	 * agents is accomplished with the communicator. when agents communicate they place messages on to the
	 * message queue of another agent. a message tells the agent to perform an action from its actionset.
	 *
	 *  @todo copy constructor for agents.
	 *  @todo make the outbox a queue
	 *
	 *  @bug
	 *
	 *  @author Garrit Jentsch
	 *
	 *  @date Oct 12th, 2009 last edited: Oct 14h, 2009 by Garrit and Michael
	 */
	class Agent {
		public:
			/// constructor
			Agent();
			Agent(int agent_id);

			/// constructor for parallel implementation
			Agent(int agent_id, int node_id);

			/// destructor
			virtual ~Agent();

			/** set the parent of this agent
			 * @param parent_id id of the parent
			 */
			virtual void setParent(int parent_id, int node_id=0);

			/** add a child of this agent
			 * @param child_id id of child to add
			 */
			virtual void addChild(int child_id, int node_id=0);

			/** adds an action to the actionset
			 * @param a action to be added to actionset
			 */
			void addAction(Action* a);

			/** adds communicator to the system
			 * @param c communicator to be added
			 */
			void addCommunicator(Communicator *c);

			/** adds a message generator to the agent
			 * @param msg messagegenerator
			 */
			void addMessageGenerator(MessageGenerator *msg);

			/** adds a message generator for initialisation messages
			 * @parem msg messagegenerator
			 */
			void addMessageGeneratorForInitialisation(MessageGenerator *msg);

			/** adds a simulator to the simulators vector
			 * @param s simulator to be added
			 */
			virtual void addSimulator(Simulator *s);

			/** adds a special agent to this agent
			 * @param special_id id of special agent to be added
			 */
			void addSpecialAgent(int special_id, int node_id=0);

			/**
			 * method for accessing database of an agent
			 * @return pointer to database
			 */
			Database* getDatabase();

			/** return parent agent ID
			 * @return ID of the parent agent
			 */
			virtual int getParentId();

			int getParentNode();

			/** return this agent ID
			 * @return ID of the parent agent
			 */
			int getAgentId();

			/** get the number of child agents
			 * @return the number of children
			 */
			virtual int getNumOfChildren();

			/** return the ID of the child agent at the given index
			 * @param childIndex index of the child agent ID to return
			 */
			virtual int getChildAgentId(int childIndex);

			virtual int getChildAgentNode(int childIndex);

			/** sends of messages to parent for initialisation
			 */
			virtual void init();

			/** check if the given ID is a child of this
			 * @return true if the agentID given is a child, false otherwise childIndex index of the child agent ID to return
			 */
			virtual bool isChildAgent(int agentId);

			/** outputs the current state of the agent by dumbing its database
			 * @param ow pointer to outputwriter who does the actual job
			 */
			void outputCurrentState(OutputWriter *ow);

			void placeMessageOnMessageQueue(Message* m) { message_queue.push(m); }

			void placeMessageInOutbox(Message* m) { this->outbox.push_back(m); }

			/// propagates the agent for a timestep
			virtual void propagate(double time);

			/** method for initializing the database. again the actual job is being done by the database-initializer
			 * @param *db_init pointer to database initializer
			 */
			void setupDatabase(); //DatabaseInitializer *db_init);

			void setAgentID(int id) {
				this->agent_id = id;
			};

			void setNodeID(int id) {
				this->node_id = id;
			};

			/// causes the agent to perform the actions placed on the message queue.
			void evaluateMessageQueue();


			//////// METHODS THAT SPLIT UP THE OPERATIONS OF PROPAGATE
			void placePreMessagesInOutbox();
			void placePostMessagesInOutbox();
			void sendMessages();
			void advanceSimulators(double dt);


		protected:
			/// for remembering the first name ...
			int initial_node_id;
			int initial_agent_id;

			/// id of node on which agent lives
			int node_id;

			/// unique identifyer of this agent
			int agent_id;

			/// unique identifyer of the parent agent
			int parent_id;
			int parent_node_id;

			/// unique identifyers of the child agents
			vector<int> children_id;
			vector<int> children_node_id;

			/// unique identifyers of the special agents
			vector<int> special_agents_id;
			vector<int> special_agents_node_id;

			/// database of the agent
			Database *db;

			/// communicator for exchanging messages with the agent's parent agent andchildren
			Communicator *c;

			/// message queue; inbox of an agent
			queue <Message* > message_queue;

			/// vector of simulators
			vector <Simulator *> simulators;

			/// actionset. a message tells the agent to execute an action from this set
			vector <Action *> action_set;

			/// message outbox of agent.
			vector <Message *> outbox;

			/// vector of pointers to message_generators
			vector <MessageGenerator* > message_generators;

			/// vector of message generators for initialisation purposes
			vector<MessageGenerator* > message_generators_for_initialisation;

		private:
			static unsigned int AgentCounter;
	};
}

#endif /* AGENT_HH_ */
