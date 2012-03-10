/**
 * @file communicator.hh
 *
 * this file contains the interface to the abstract class communicator.
 *  Created on: Oct 12, 2009
 *      Author: jentsch
 *
 *      @author Garrit Jentsch
 *      @date Oct 12, 2009 last edited: Oct 14, 2009 by Garrit and Michael
 */

#ifndef COMMUNICATOR_HH_
#define COMMUNICATOR_HH_

#include <iostream>

#include "../agent/message/message.hh"

using namespace std;

namespace Hive
	{

	/**
	 * @brief communicator class for agent communication
	 *
	 * the communicator hides the exchange of messages from the agent. this is necessary because an
	 * agent should not know whether it is run on a single machine or on a parallel environment. does a
	 * communicator need to know for which agent it is responsible?!
	 *
	 * @todo everything
	 *
	 * @bug none so far
	 *
	 * @author Garrit Jentsch
	 * @date Oct 12, 2009 last edited: Oct 14, 2009 by Garrit and Michael
	 */

	class Message;

	class Communicator {
	public:
		/// constructor
		Communicator() {};

		/// destructor
		virtual ~Communicator() {};


		virtual void clearQueues() {};

		/// method for sending/recieving accumulated messages to other nodes
		virtual void parallelCommunicate() {};


		/** method for sending messages
		 * @param m message to be send
		 * @param dest identifyer of destination
		 */
		virtual void sendMessage(Message *m)=0;

		virtual void propogateAgent(int agentId, double time)=0;

	};
}

#endif /* COMMUNICATOR_HH_ */
