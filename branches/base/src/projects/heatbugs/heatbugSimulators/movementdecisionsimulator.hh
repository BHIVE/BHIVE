/*
 * movementdecisionsimulator.hh
 *
 *  Created on: Jan 5, 2011
 *      Author: jentsch
 */

#ifndef MOVEMENTDECISIONSIMULATOR_HH_
#define MOVEMENTDECISIONSIMULATOR_HH_

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
	 * @brief simple simulator with which the bug decides into which direction it should move
	 *
	 * @author Garrit Jentsch
	 *
	 * @date Jan 5th, 2011
	 */
	class MovementDecisionSimulator : public Simulator {
	public:
		MovementDecisionSimulator();

		~MovementDecisionSimulator();

		void step(double t);

		void prepare();

		void setAgent(Agent *ag);

		void setTimeStep(double);

		void synchroniseWithDatabase();

	protected:
		void initialise();

	private:
		// timestep
		double dt;

		TVectorData<DoubleData> *local_temperature_around_me;
		double  				 preferred_temperature;
		IntegerData             *move_direction;
	};
}

#endif /* MOVEMENTDECISIONSIMULATOR_HH_ */
