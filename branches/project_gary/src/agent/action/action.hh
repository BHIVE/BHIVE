/*
 * @file action.hh
 *
 * this file contains the interface to the action class.
 *
 * @author Garrit Jentsch
 *
 * @date Oct 13th, 2009, last edited: Oct 14th, 2009 by Garrit and Michael
 *
 *
 */

#ifndef ACTION_HH_
#define ACTION_HH_

#include "agent.hh"
#include "data/data.hh"

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
		/// destructor
		Action() {};
		/// destructor
		virtual ~Action() {};

		/**
		 * executes an action
		 * @param d pointer to data-object containing the argument for the action
		 */
  		virtual void fire(Agent* ag,Data *d,int source)=0;
	};
}

#endif /* ACTION_HH_ */
