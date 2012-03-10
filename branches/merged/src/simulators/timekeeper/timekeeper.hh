/*
 * celltimekeeper.hh
 *
 *  Created on: Jul 8, 2010
 *      Author: msneddon
 */

#ifndef TIMEKEEPER_HH_
#define TIMEKEEPER_HH_


#include <iostream>
#include <map>
#include <vector>

#include "../simulator.hh"
#include "../../agent/data/primitive/primitiveData.hh"
#include "../../agent/data/template/tdata.hh"
#include "../../agent/data/grid/grid.hh"
#include "../../util/util.hh"

namespace Hive {

	/**
	 * Simple simulator that increments the cell time at each step - this is so that
	 * the cell time is incremented independently of all other simulators (could this
	 * be added directly in the agent class?
	 */
	class TimeKeeper : public Simulator {
	public:
		/// Constructor
		TimeKeeper(Agent *a, string clockName);

		/// destructor
		~TimeKeeper();

		/**
		 * integrate the system for a timestep
		 * @param dt timestep
		 */
		void step(double dt);

		/**
		 * prepare simulator for the first time, this will call
		 * the connectToDataBase and initialise methods
		 */
		void prepare();

		/**
		 * update simulator's internal variables before executing the integrate command
		 */
		void synchroniseWithDatabase();

		void setAgent(Agent *ag);

	protected:
		/**
		 *  initialise simulator's internal variables for the first time
		 */
		void initialise();



	private:
		Database *db;
		DoubleData *cell_time;
		DoubleData *dt;
		string clockName;
	};
}


#endif /* TIMEKEEPER_HH_ */
