/*
 * deluettenschieter.hh
 *
 * simulator for environment for chemotaxis project.
 * directly translated de luetten schieter means the little sh...
 * hence this simulator simulates the dropping of point sources into the environement
 *
 *  Created on: Sep 13, 2010
 *      Author: jentsch
 */

#ifndef DELUETTENSCHIETERSIMULATOR_HH_
#define DELUETTENSCHIETERSIMULATOR_HH_

#include <cmath>
#include <iostream>
#include <map>
#include <vector>

#include "../simulator.hh"
#include "../../agent/data/mathfunction/mathfunctiondata.hh"
#include "../../agent/data/primitive/primitiveData.hh"
#include "../../agent/data/template/tdata.hh"
#include "../../util/util.hh"

using namespace std;

namespace Hive
{
	// simulator for the world. it calculates how much of the desired
	// input a cell actually is allowed to get.
	class DeLuettenSchieterSimulator : public Simulator {
	public:
		/// Constructor
		DeLuettenSchieterSimulator(Agent *ag);

		/// destructor
		~DeLuettenSchieterSimulator();

		/**
		 * integrate the system for a timestep
		 * @param t timestep
		 */
		void step(double t);

		void step(Agent *ag, double t);

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

		void setEquilibrationTime(double t) {
			this->equilibration_time = t;
		};

		void runTest();


	protected:
		/**
		 *  initialise simulator's internal variables for the first time
		 */
		void initialise();



	private:
		Database *db;

		// self-explanatory parameters
		/// i guess it would make sense to use double pointers instead of DoubleData ...
		DoubleData *timeparameter;
		DoubleData *distanceparameter;
		DoubleData *time;
		DoubleData *time_of_next_shitting;
		TVectorData<MathFunctionData* > *tretmienen;
		TVectorData<double> *lastposition;
		TVectorData<double> *birthdays_of_tretmienen;
		DoubleData *initial_mean;
		DoubleData *initial_sigma;
		DoubleData *chemo_diff;
		BoolData   *switch_for_new_drops;

		double equilibration_time;
	};
}

#endif /* DELUETTENSCHIETER_HH_ */
