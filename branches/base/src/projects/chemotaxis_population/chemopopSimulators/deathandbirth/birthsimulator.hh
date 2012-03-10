/*
 * birthsimulator.hh
 *
 *  Created on: Jun 21, 2010
 *      Author: jentsch
 */

#ifndef BIRTHSIMULATOR_HH_
#define BIRTHSIMULATOR_HH_


#include <iostream>
#include <map>
#include <vector>

#include "../../../../core/simulators/simulator.hh"
#include "../../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../../core/agent/data/template/tdata.hh"
#include "../../../../core/util/util.hh"

#include "../../chemopopData/grid/grid.hh"

namespace Hive {
	class BirthSimulator : public Simulator {
	public:
		/// Constructor
		BirthSimulator(Agent *a);

		/// destructor
		~BirthSimulator();

		/**
		 * integrate the system for a timestep
		 * @param t timestep
		 */
		void step(double t);

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
		DoubleData *essence_threshold_for_birth;
		BoolData *birth_flag;
	};
}


#endif /* BIRTHSIMULATOR_HH_ */
