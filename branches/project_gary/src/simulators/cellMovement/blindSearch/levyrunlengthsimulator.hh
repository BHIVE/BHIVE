/*
 * levyrunlengthsimulator.hh
 *
 *  Created on: Jul 7, 2010
 *      Author: jentsch
 */

#ifndef LEVYRUNLENGTHSIMULATOR_HH_
#define LEVYRUNLENGTHSIMULATOR_HH_

#include <cmath>

#include "../../../agent/data/data.hh"
#include "../../../agent/data/template/tdata.hh"
#include "../../../agent/data/primitive/primitiveData.hh"

#include "../../../agent/messageGenerator/chemopopMessageGenerator/chemopopMessageGenerator.hh"
#include "../../../projects/chemotaxis_population/cellparameterinitializer.hh"

#include "../../simulator.hh"
#include "../../../util/util.hh"

#include "blindagentmovementsimulator.hh"

using namespace std;
using namespace Hive;

namespace ChemoPop {
	class LevyRunLengthSimulator : public BlindAgentMovementSimulator {
	public:
		LevyRunLengthSimulator();
		~LevyRunLengthSimulator();

	protected:
		void drawNewDistance();
	private:
		double lmin;
		double lmax;
	};


}



#endif /* LEVYRUNLENGTHSIMULATOR_HH_ */
