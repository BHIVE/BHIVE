/*
 * cubeconstantinitialiser.hh
 *
 *  Created on: Jun 21, 2010
 *      Author: jentsch
 */

#ifndef CUBECONSTANTINITIALISER_HH_
#define CUBECONSTANTINITIALISER_HH_


#include "nutrientinitialiser.hh"
#include "../../agent/data/grid/grid.hh"
#include "../../util/util.hh"

namespace ChemoPop {
class CubeConstantInitialiser : public NutrientInitialiser {
	public:
		CubeConstantInitialiser();
		virtual ~CubeConstantInitialiser();

		virtual void initialise();

		void setValue(double v);
		void setGrid(Grid* g) { this->cube = g; };

	protected:

		Grid *cube;
		double v;
	};

}

#endif /* CUBECONSTANTINITIALISER_HH_ */
