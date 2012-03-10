/*
 * @file serialcommunicator.hh
 *
 *this file contains the interface of the serialcommunicator
 *
 *@date Oct 14, 2009 last edited 10-15-2009
 *@author garrit
 *@author jentsch
 */

#ifndef SERIALCOMMUNICATOR_HH_
#define SERIALCOMMUNICATOR_HH_

#include "../communicator.hh"
#include <map>

using namespace std;


namespace Hive
	{

	/**
	 * @brief serialcommunicator implements the communicator
	 *
	 * a serial communicator is needed to exchange messages among agent for the case that the entire
	 * simulation lives on a single cpu.
	 *
	 * @todo
	 *
	 * @author garrit jentsch
	 * @date Oct 14, 2009, last edited 10-15-2009
	 */

	class SerialCommunicator : public Communicator {
	public:
		/// constructor
		SerialCommunicator();

		/// destructor
		~SerialCommunicator();

		void receiveMessages();

		void sendMessage(Message* m);

		/**
		 * adds an agent to the communication map
		 * @param a pointer to agent to be added to map
		 */
		void addAgent(Agent *a);

		/**
		 * sets up the lookup table
		 * @param c lookup table
		 */
		void setupCommunicationMap(map<int, Agent *>& c);

		void propogateAgent(int agentId, double time);


	private:
		/// lookup table such that the communicator knows to which agent it has to send a message
		map<int, Agent*> connection_map;
	};
}

#endif /* SERIALCOMMUNICATOR_HH_ */
