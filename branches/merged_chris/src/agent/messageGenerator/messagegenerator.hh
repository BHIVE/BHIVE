/*
 * @file messagegenerator.hh
 *
 * this file contains the interace to the messagegenerator class.
 *
 * @author Garrit Jentsch
 *
 * @date Oct 13th, 2009 last edited: 10-14-2009 by garrit and michael
 *
 */

#ifndef MESSAGEGENERATOR_HH_
#define MESSAGEGENERATOR_HH_

#include "../agent.hh"
#include "../message/message.hh"

using namespace std;


namespace Hive
{

	/**
	 * @brief class for generating messages
	 *
	 * this virtual class by an agent to generate messages that it wants to send to its children and parent agent.
	 *
	 * @todo everything
	 *
	 * @bugs none so far
	 *
	 * @author Garrit Jentsch and Michael Sneddon
	 *
	 * @date Oct 13th, 2009 last edited: 09-10-2010 by garrit
	 */

	class Agent;
	class Message;

	class MessageGenerator {
	public:
		/// default constructor
		MessageGenerator(){ };

		/**
		 * construct
		 * @param ag pointer to source agent
		 */
		MessageGenerator(Agent *ag, bool isParentMessage) {
			this->source = ag;
			this->isParentMessage = isParentMessage;
		} ;

		/**
		 * constructor
		 * @param isPM flag to identify whether message is for parent
		 */
		MessageGenerator(bool isPM) {
			this->source = NULL;
			this->isParentMessage = isPM;
		};

		bool isMessageForParent() { return isParentMessage; };

		/// by default a message generator is never a message generator for generating
		/// a message to a special agent
		virtual bool isMessageForSpecialAgent() { return false; };

		/// destructor
		virtual ~MessageGenerator() {};

		/**
		 * generate message and puts it into the outbox of the source agent
		 * @param dest agentid to which a message needs to be send
		 */
		virtual void placeMessage(int destID)=0;

		/**
		 * generates a message and puts it into the outbox of the agent that is
		 * passed as an argument to the message generator
		 *  @param ag pointer to source agent
		 *  @param dest agentid to which a message needs to be send
		 */
		virtual void placeMessage(Agent *ag, int destID){};

	protected:
		/// pointer to agent that generates the message
		Agent *source;
		bool isParentMessage;

	};
}

#endif /* MESSAGEGENERATOR_HH_ */
