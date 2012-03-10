/*
 * levyrunlengthsimulator.hh
 *
 *  Created on: Jul 7, 2010
 *      Author: jentsch
 */

#ifndef LEVYRUNLENGTHSIMULATOR_HH_
#define LEVYRUNLENGTHSIMULATOR_HH_

#include <cmath>

#include "../../../../../core/agent/data/data.hh"
#include "../../../../../core/agent/data/template/tdata.hh"
#include "../../../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../../../core/simulators/simulator.hh"
#include "../../../../../core/util/util.hh"

#include "../../../chemopopMessageGenerator/chemopopMessageGenerator.hh"
#include "../../../chemopopInitialiser/cellparameterinitializer.hh"

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
