/*
 * NutrientInitialiser.hh
 * abstract class for initialising the environment with nutrients
 *
 *
 *  Created on: Jun 17, 2010
 *      Author: jentsch
 */

#ifndef NUTRIENTINITIALISER_HH_
#define NUTRIENTINITIALISER_HH_

#include "../../agent/data/grid/grid.hh"

using namespace Hive;
using namespace std;

namespace ChemoPop {

	class NutrientInitialiser {
	public:
		// constructor
		NutrientInitialiser() {};

		// destructor
		virtual ~NutrientInitialiser() {};

		// only method that is really needed. does the initialisation
		// of the initialisation
		virtual void initialise() = 0;
	};
}
#endif /* NUTRIENTINITIALISER_HH_ */
