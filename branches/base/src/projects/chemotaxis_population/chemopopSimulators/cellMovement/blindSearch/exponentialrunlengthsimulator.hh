/*
 * exponentialrunlengthsimulator.hh
 *
 *  Created on: Jul 7, 2010
 *      Author: jentsch
 */

#ifndef EXPONENTIALRUNLENGTHSIMULATOR_HH_
#define EXPONENTIALRUNLENGTHSIMULATOR_HH_


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
	class ExponentialRunLengthSimulator : public BlindAgentMovementSimulator {
	public:
		ExponentialRunLengthSimulator();
		~ExponentialRunLengthSimulator();

	protected:
		void drawNewDistance();
	};


}

#endif /* EXPONENTIALRUNLENGTHSIMULATOR_HH_ */
