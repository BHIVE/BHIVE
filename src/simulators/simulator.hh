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
	 * an abstract class and the individual simulators have to be derived from this class.
	 *
	 * @todo everything
	 *
	 * @bug
	 *
	 * @author Garrit Jentsch
	 *
	 * @date October 13th, 2009 last edited: 10-14-2009 by Garrit and Michael
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
		virtual void step(double t)=0;

		/**
		 * prepare simulator for the first time, this will call
		 * the connectToDataBase and initialise methods
		 */
		virtual void prepare()=0;

		/**
		 * update simulator's internal variables before executing the integrate command
		 */
		virtual void synchroniseWithDatabase()=0;

		/**
		 * set the simulator's agent.  you must implement this so that you can double check
		 * that your agent has the correct type of database set up.
		 */
		virtual void setAgent(Agent *ag) { this->agent = ag; };


	protected:



		/**
		 *  initialise simulator's internal variables for the first time
		 */
		virtual void initialise()=0;

		Agent *agent;

	};
}
#endif /* SIMULATOR_HH_ */
