/*
 * @file arnie.hh
 *
 * this file contains the interface to the action class.
 *
 * @author Garrit Jentsch
 *
 * @date Oct 13th, 2009, last edited: Oct 14th, 2009 by Garrit and Michael
 *
 *
 */

#ifndef ARNIE_HH_
#define ARNIE_HH_

#include "../agent.hh"
#include "../data/data.hh"

using namespace std;


namespace Hive
{
	class Agent;

	/**
	 * @brief action class for working on an agent
	 *
	 * actions are standardised methods that manipulate the dataset of an agent. a message
	 * tells the agent to perform an action.
	 *
	 * @see message
	 *
	 * @todo everything. the method fire of an action should get the identifyer of the agent, that initiated the action
	 *
	 * @bug
	 *
	 * @author Garrit Jentsch
	 *
	 * @date Oct 13th, 2009 last edited: Oct 14th, 2009 by Garrit and Michael
	 */

	class Action {
	public:
		/// default constructor
		Action() {};

		/**
		 * constructor
		 * @param i unique_identifyer
		 */
		Action(int i) {this->unique_identifyer = i;};

		/// destructor
		virtual ~Action() {};

		/**
		 * set agent on which this action is supposed to work.
		 * @param *ag pointer to agent
		 */
		void setAgent(Agent *ag) { this->agent = ag;};

		/**
		 * sets identifyer of this action
		 * @param id identifyer
		 */
		void setIdentifyer(int id) { this->unique_identifyer = id;};

	/**
	 * executes an action
	 * @param d pointer to data-object containing the argument for the action
	 */
  	virtual void fire(Data *d)=0;

	protected:
		/// identifyer of an action
		int unique_identifyer;

		/// pointer to agent on which action works
		Agent *agent;
	};
}

#endif /* ARNIE_HH_ */
