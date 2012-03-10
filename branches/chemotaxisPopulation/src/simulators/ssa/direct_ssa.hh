/*
 * @file direct_ssa.hh
 *
 * this file contains the interface of direct method ssa
 *
 * @date Dec 16, 2009
 *
 * @author garrit jentsch
 */

#ifndef DIRECT_SSA_HH_
#define DIRECT_SSA_HH_

#include "../simulator.hh"
#include "../../agent/data/rate/grate.hh"
#include "../../agent/data/template/tdata.hh"
#include "../../agent/data/primitive/primitiveData.hh"
#include "../../util/rand/rand.hh"

#include <cmath>
#include <iostream>

using namespace std;

namespace Hive {

	/**
	 * @brief implementation of the direct method of the gillespie algorithm
	 *
	 * @date December, 16th  2010 last edited 10-16-2009
	 *
	 * @author Garrit Jentsch
	 *
	 */
	class DirectSSA : public Simulator {
	public:
		/// default constructor
		DirectSSA();

		/// destructor
		~DirectSSA();

		/// join simulator with an agent
		void setAgent(Agent *ag);

		/// perform a timestep of length t
		void step(double t);

		/// in this method one decides whether one want to set up a dependency graph
		void prepare();

		/// this recalculates the propensities vector before starting the simulator again
		void synchroniseWithDatabase();

		/// testing routine of the algorithm
		void testing();

		/// for debugging generate some output
		void printContent(ostream& out);

	protected:
		/// this should set up the dependency graph;
		void initialise();

		/// calculates total propensity of the system
		void calcTotalPropensity();

		/// recalculate the propensities of the reactions affected by the last reaction
		void updatePropensity(int fired_reaction);

	private:
		Agent *ag;

		/// agent's volume
		DoubleData *agent_volume;

		/// pointer to the propensities of the reactions
		TVectorData<double> *propensities;

		/// reactions
		TVectorData<GRate*> *reactions;

		/// lookup table for effectively updating the propensities
		vector<vector<int> > dependency_graph;

		/// pointer to the particle number vector
		TVectorData<int> *particle_numbers;

		/// pointer to the total propensity of the system
		DoubleData *total_propensity;

		/// internal time of the agent
		DoubleData *time;

		/// next reaction time
		DoubleData *nxt;

		double old_propensity;
	};

}

#endif /* DIRECT_SSA_HH_ */
