/*
 * theGreatCommunicator.hh
 *
 *  Created on: Feb 24, 2010
 *      Author: msneddon
 */

#ifndef REAGAN_HH_
#define REAGAN_HH_


#include "../communicator.hh"
#include "../../parallel/parallel.hh"
#include <map>

using namespace std;


namespace Hive
{




	class Reagan : public Communicator {
	public:
		/// constructor
		Reagan();

		/// destructor
		~Reagan() {};


		void clearQueues();

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




		/// method for sending/recieving accumulated messages to other nodes
		void parallelCommunicate();

		/// need this, otherwise abstract class
		void propogateAgent(int agentId, double time) { } ;

	private:
		/// lookup table such that the communicator knows to which agent it has to send a message
		map<int, Agent*> localConnectionMap;


		vector <int> overseasDestinations;

		vector <int> alreadyinthere;

		vector <Message *> overseasMessageQueue;
		vector <Message *> overseasReceivingQueue;

	};




}



#endif /* THEGREATCOMMUNICATOR_HH_ */
