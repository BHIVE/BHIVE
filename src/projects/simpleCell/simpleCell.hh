/**
 * simpleCell.hh
 *
 *  Created on: Jan 29, 2010
 *      Author: msneddon
 */

#ifndef SIMPLECELL_HH_
#define SIMPLECELL_HH_



#include <iostream>


#include "composers/SC_composers.hh"

#include "simulators/SC_simulators.hh"
#include "agentFactories/SC_agentFactories.hh"
#include "actions/SC_actions.hh"
#include "messageGenerators/SC_messageGenerators.hh"

using namespace std;
using namespace Hive;


namespace SimpleCell {


	void runSimpleCell(int argc, char *argv[]);

    class SimpleCellConstants {
		public:
			const static int CONSTANT_ENVIRONMENT;
			const static int LINEAR_ENVIRONMENT;
			const static int EXPONENTIAL_ENVIRONMENT;
			const static int CAPILLARY_ENVIRONMENT;


			const static int NOISE_SIGNALING;
			const static int SIMPLE_WINGREEN_SIGNALING;

			const static int SINGLE_MOTOR;
			const static int VOTING_MOTORS;
			const static int CONFORMATION_MOTORS;

			const static int STANDARD_SWIMMING;
			const static int TURNER_TUMBLE_ANGLE_SWIMMING;


			// constants for the motor and multiple flagella models
			const static int CCW;
			const static int CW;

			const static int RUN;
			const static int TUMBLE;

			const static int NORMAL;
			const static int SEMI;
			const static int CURLY;
			const static int NONE;

	};


}




#endif /* SIMPLECELL_HH_ */
