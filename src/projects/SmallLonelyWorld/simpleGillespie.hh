/*
 * @file simpleGillespie.hh
 *
 *	A simple (unoptimized) Gillespie simulator
 *
 *	@author Leonard Harris
 *
 *	@date October 20, 2009, last edited: October 20, 2009
 */

#ifndef SIMPLEGILLESPIE_HH_
#define SIMPLEGILLESPIE_HH_

#include "../../simulators/simulator.hh"
#include "../../agent/data/primitive/primitiveData.hh"
#include "../../util/util.hh"
#include "simpleSpecies.hh"
#include "simpleRxn.hh"
#include "ourSimulator.hh"

using namespace std;

namespace Hive{

	class SimpleGillespie : public OurSimulator {

	public:
		/// Constructor
		SimpleGillespie(int nSpecies, SimpleSpecies **species, int nRxn,
				SimpleRxn **reaction, bool debug);

		/// Destructor
		~SimpleGillespie();

		/**
		 * integrate the system for a given period of time t
		 * @param t time interval
		 */
		void step(double t);

		double nextStep();

		double nextStep(double maxTau);

		void run(double startTime, double endTime);

		void run(double endTime);

		void run(int nSteps);

		void setAgent(Agent* a);

		/**
		 * prepare simulator for the first time, this will call
		 * the connectToDataBase and initialise methods
		 */
		void prepare();

		/**
		 * update simulator's internal variables before executing the integrate command
		 */
		void synchroniseWithDatabase();

	protected:
		/**
		 *  initialise simulator's internal variables for the first time
		 */
		void initialise();

	private:
		Database *db;
		int nSpecies;
		SimpleSpecies **species;
		int nRxns;
		SimpleRxn **reaction;
		bool debug;
	};
}

#endif /* SIMPLEGILLESPIE_HH_ */
