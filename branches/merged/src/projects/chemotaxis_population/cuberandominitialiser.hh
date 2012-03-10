/*
 * cuberandominitialiser.hh
 *
 *  Created on: Jun 17, 2010
 *      Author: jentsch
 */

#ifndef CUBERANDOMINITIALISER_HH_
#define CUBERANDOMINITIALISER_HH_

#include "nutrientinitialiser.hh"
#include "../../agent/data/grid/grid.hh"
#include "../../util/util.hh"

using namespace Hive;
using namespace std;

namespace ChemoPop {

	class CubeRandomInitialiser : public NutrientInitialiser {
	public:
		CubeRandomInitialiser();
		virtual ~CubeRandomInitialiser();

		virtual void initialise();

		void setNumberOfPoints(int num);
		void setNutrientRange(double minimum, double maximum);

		void setGrid(Grid* g) { this->cube = g; };
	protected:

		Grid *cube;
		int number_of_points;
		double minimum;
		double maximum;
	};


}

#endif /* CUBERANDOMINITIALISER_HH_ */
