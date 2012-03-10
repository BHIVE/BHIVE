/*
 * simulatorforresolvingconflicts.hh
 *
 *  Created on: Jan 5, 2011
 *      Author: jentsch
 */

#ifndef SIMULATORFORRESOLVINGCONFLICTS_HH_
#define SIMULATORFORRESOLVINGCONFLICTS_HH_

#include "../../../core/simulators/simulator.hh"
#include "../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../core/agent/data/template/tdata.hh"
#include "../../../core/util/util.hh"

#include <cmath>
#include <iostream>
#include <vector>

using namespace std;
using namespace Hive;

namespace Heatbug {

	/*
	 * @brief simple simulator for resolving conflict in the occupation of the bug grid
	 *        see also comments in cpp file.
	 *
	 * @author Garrit Jentsch
	 *
	 * @date Jan 5th, 2011
	 */
	class SimulatorForResolvingConflicts : public Simulator {
	public:
		SimulatorForResolvingConflicts();

		~SimulatorForResolvingConflicts();

		// checks whether particles at pos x,y have been moved
		bool checkMoveFlags(int x, int y);

		// tells a bug to go back to its old position
		void getBack(int bug_index);

		// perform a single step
		void step(double t);

		void prepare();

		void setAgent(Agent *ag);

		void setTimeStep(double);

		void synchroniseWithDatabase();

		void test();
	protected:
		void initialise();

	private:
		double dt;
		int    num_bugs;
		int    gridsize;

		TVectorData<TVectorData<IntegerData>* >                *bug_grid;
		TVectorData<TVectorData<IntegerData>* > 			   *bug_positions;

		TVectorData<TVectorData<IntegerData>* >				   *desired_bug_positions;
		TVectorData<TVectorData<TVectorData<IntegerData>* >* > *desired_bug_grid;

		// temporary data structures needed by the simulator.
		TVectorData<TVectorData<int>* > *tmp_bug_grid;
		TVectorData<TVectorData<int>* > *tmp_bug_positions;
		TVectorData<int>               *tmp_bug_moved;
	};
}

#endif /* SIMULATORFORRESOLVINGCONFLICTS_HH_ */
