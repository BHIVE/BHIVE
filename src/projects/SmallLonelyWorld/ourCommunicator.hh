/*§
 * simpleCommunicator.hh
 *
 *  Created on: Oct 26, 2009
 *      Author: Len
 */

#ifndef OURCOMMUNICATOR_HH_
#define OURCOMMUNICATOR_HH_

#include "../../communicator/serial/serialcommunicator.hh"

namespace Hive{

	class OurCommunicator : public SerialCommunicator {
	public:

		/// constructor
		OurCommunicator();

		/// destructor
		~OurCommunicator() {};

		/// method for receiving messages
		void receiveMessages();

		/** method for sending messages
		 * @param m message to be send
		 * @param dest identifyer of destination
		 */
		void sendMessage(Message *m, int dest);

		void propogateAgent(int agentId, double time);

		void setAgent(Agent *a);

	protected:

		Agent *agent;

	};

}


#endif /* OURCOMMUNICATOR_HH_ */
