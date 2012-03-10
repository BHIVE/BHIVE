/*
 * metabolismforblindagent.hh
 *
 *  Created on: Jun 21, 2010
 *      Author: jentsch
 */

#ifndef METABOLISMFORBLINDAGENT_HH_
#define METABOLISMFORBLINDAGENT_HH_

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
	class MetabolismForBlindAgentSimulator : public Simulator {
	public:
		/// Constructor
		MetabolismForBlindAgentSimulator(Agent *a);

		/// destructor
		~MetabolismForBlindAgentSimulator();

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

		DoubleData *essence;
		DoubleData *mass;
		DoubleData *kcat;
		DoubleData *Km;
		DoubleData *essence_cost_for_movement;
		DoubleData *velocity;
		DoubleData *r1;



//		// local copies to parts of the world's agents database
//		DoubleData          *cost_for_living;
//		TVectorData<double> *accumulated_ligands;
//		DoubleData          *velocity;
//		DoubleData			*cost_for_movement;
	};
}

#endif /* METABOLISMFORBLINDAGENT_HH_ */
