/*
 * birthsimulator.hh
 *
 *  Created on: Jun 21, 2010
 *      Author: jentsch
 */

#ifndef DEATHSIMULATOR_HH_
#define DEATHSIMULATOR_HH_


#include <iostream>
#include <map>
#include <vector>

#include "../simulator.hh"
#include "../../agent/data/primitive/primitiveData.hh"
#include "../../agent/data/template/tdata.hh"
#include "../../agent/data/grid/grid.hh"
#include "../../util/util.hh"

namespace Hive {
	class DeathSimulator : public Simulator {
	public:
		/// Constructor
		DeathSimulator(Agent *a);

		/// destructor
		~DeathSimulator();

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

		// local copies to parts of the world's agents database
		DoubleData *essence;
		DoubleData *essence_threshold_for_death;
		DoubleData *background_death_rate_per_unit_time;
		BoolData *death_flag;
		BoolData *birth_flag;
	};
}


#endif /* DEATHSIMULATOR_HH_ */
