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
	class Database;
	class Simulator {
	public:
		Simulator() {subSim = NULL;db = new Database();};
		virtual ~Simulator() {};
		/**
		 * integrate the system for a timestep
		 * @param t timestep
		 */
		virtual void step(Agent* inAgent,double t)=0;
		void setSubSimulator(Simulator* subSimulator) {subSim = subSimulator;};
		Simulator* getSubSim() {return subSim;};
		Database* getDatabase();
	protected:
		/**
		 *  initialise simulator's internal variables for the first time
		 */
		virtual void initialise()=0;
		Simulator* subSim;
		Database *db;
	};
}
#endif /* SIMULATOR_HH_ */
