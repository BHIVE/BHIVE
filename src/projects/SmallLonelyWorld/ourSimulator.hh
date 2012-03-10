/*
 * ourSimulator.hh
 *
 *  Created on: Oct 27, 2009
 *      Author: Len
 */

#ifndef OURSIMULATOR_HH_
#define OURSIMULATOR_HH_

#include "../../simulators/simulator.hh"

using namespace std;

namespace Hive{

	class OurSimulator : public Simulator{
	public:

		OurSimulator();
		~OurSimulator(){}

		void setAgent(Agent *a);

	};
}

#endif /* OURSIMULATOR_HH_ */
