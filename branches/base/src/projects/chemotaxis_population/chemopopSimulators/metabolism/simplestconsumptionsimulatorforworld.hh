/*
 * simplestconsumptionsimulator.hh
 *
 *  Created on: Jun 18, 2010
 *      Author: jentsch
 */

#ifndef SIMPLESTCONSUMPTIONSIMULATORFORWORLD_HH_
#define SIMPLESTCONSUMPTIONSIMULATORFORWORLD_HH_

#include <iostream>
#include <map>
#include <vector>

#include "../../../../core/simulators/simulator.hh"
#include "../../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../../core/agent/data/template/tdata.hh"
#include "../../../../core/util/util.hh"

#include "../../chemopopData/grid/grid.hh"

namespace Hive
{
	// simulator for the world. it calculates how much of the desired
	// input a cell actually is allowed to get.
	class SimplestConsumptionSimulatorForWorld : public Simulator {
	public:
		/// Constructor
		SimplestConsumptionSimulatorForWorld(Agent *a);

		/// destructor
		~SimplestConsumptionSimulatorForWorld();

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
		DoubleData *totalNutrient;
		TVectorData<double> *desired_cell_consumption;
		TVectorData<int> *cell_positions;
		TVectorData<int> *number_cells_per_voxel;
		Grid *cube;

	};
}

#endif /* SIMPLESTCONSUMPTIONSIMULATORFORWORLD_HH_ */
