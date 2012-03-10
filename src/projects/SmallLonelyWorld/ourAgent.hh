/*
 * ourAgent.hh
 *
 *  Created on: Oct 27, 2009
 *      Author: Len
 */

#ifndef OURAGENT_HH_
#define OURAGENT_HH_

#include "ourSimulator.hh"
#include "ourCommunicator.hh"

using namespace std;

namespace Hive{

	class OurAgent : public Agent{
	public:

		OurSimulator *simulator;
		OurCommunicator *communicator;

		OurAgent();
		OurAgent(OurSimulator *s);
		OurAgent(OurSimulator *s, OurCommunicator *c);
		~OurAgent(){}

		void addSimulator(OurSimulator *s);
		void addCommunicator(OurCommunicator *c);

	};

}

#endif /* OURAGENT_HH_ */
