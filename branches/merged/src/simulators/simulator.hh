/*
 * @file simulator.hh
 *
 *	this file contains the interface to the simulator class.
 *
 *	@author Garrit Jentsch
 *
 *	@date 13th of October 2009, last edited: 10-14-2009 by Garrit and Michael
 *
 *  Created on: Oct 13, 2009
 *      Author: jentsch
 */
#ifndef SIMULATOR_HH_
#define SIMULATOR_HH_

#include "../agent/agent.hh"

using namespace std;


namespace Hive {


	class Agent;
	/**
	 * @brief simulator class
	 *
	 * a simulator operates on an agent's dataset and propagates it through time. this is
	 * almost an abstract class and the individual simulators have to be derived from this class.
	 *
	 * a simulator can be directly connected to an agent or alternatively an agent might be passed
	 * as an argument to the simulators stepping routine. methods are virtual with a default
	 * implementation doing nothing. hence, not every method needs to be re-implemented when
	 * designing a specific simulator
	 *
	 * @todo everything
	 *
	 * @bug
	 *
	 * @author Garrit Jentsch
	 *
	 * @date October 13th, 2009 last edited: 09-09-2010 by Garrit
	 */
	class Simulator {
	public:
		/// Constructor
		Simulator() {};

		/// destructor
		virtual ~Simulator() {};

		/**
		 * integrate the system for a timestep
		 * @param t timestep
		 */
		virtual void step(double t){};

		/**
		 * integrate the system inside of agent *ag for a timestep t
		 * @param ag pointer to agent to be stepped forward in time
		 * @param t timestep
		 */
		virtual void step(Agent *ag, double t){};

		/**
		 * seems to be obsolete:
		 * prepare simulator for the first time, this will call
		 * the connectToDataBase and initialise methods
		 *
		 * @todo check whether method is being needed.
		 */
		virtual void prepare(){};

		/**
		 * update simulator's internal variables before executing the integrate command
		 */
		virtual void synchroniseWithDatabase(){};

		/**
		 * update simulator's internal variables to the data of the provided agent
		 * @param ag parameter of the agent
		 */
		virtual void synchroniseWithDatabase(Agent *ag){};

		/**
		 * set the simulator's agent.  you must implement this so that you can double check
		 * that your agent has the correct type of database set up.
		 */
		virtual void setAgent(Agent *ag) { this->agent = ag; };


	protected:
		/**
		 *  initialise simulator's internal variables for the first time
		 */
		virtual void initialise(){};

		Agent *agent;

	};
}
#endif /* SIMULATOR_HH_ */
