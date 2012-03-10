/*
 * @file dummySimulator.hh
 *
 *	a dummy simulator
 *
 *	@author Michael Sneddon
 *
 *	@date October 15, 2009, last edited: October 15, 2009
 */

#ifndef DUMMYSIMULATOR_HH_
#define DUMMYSIMULATOR_HH_


#include "../simulator.hh"
#include "../../agent/data/primitive/primitiveData.hh"
#include "../../util/util.hh"

namespace Hive
{

	class DummySimulator : public Simulator {
	public:
		/// Constructor
		DummySimulator(Agent *a);

		/// destructor
		~DummySimulator();

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
		double internalTime;
		double xPos;
		double yPos;
		Database *db;

	};
}

#endif /* DUMMYSIMULATOR_HH_ */
