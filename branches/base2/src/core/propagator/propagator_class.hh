/*
 * propagator_class.hh
 *
 *  Created on: Feb 24, 2010
 *      Author: jentsch
 */

#include "../agent/agent.hh"
#include "../composer/composer.hh"

#ifndef PROPAGATOR_CLASS_HH_
#define PROPAGATOR_CLASS_HH_

namespace Hive {

/**
 * @brief abstract class for running a simulation. derive a class from here and you ...
 *
 */

class PropagatorClass {
public:
	PropagatorClass(Composer *composer) {
		//cerr << "# hello, i am the base propagatorClass" << endl;
		this->comp = composer;
		this->communicator = composer->getCommunicator();
		this->currentTime = 0;

		for (unsigned int ag=0; ag<comp->getNumberAgents(); ag++)
		{
			this->agents.push_back(this->comp->getAgent(ag));
		}
	};

	virtual ~PropagatorClass() {
	//	cerr << "# entering propagator_class::destructor" << endl;
	//	cerr << "# not doing nothing" << endl;
	//	cerr << "# leaving propagator_class::destructor" << endl;
	};

	virtual void go(double dt, double endTime)=0;

	virtual void equilibrate(double time)=0;

	virtual void setup()=0;

protected:
	// vector of agents that live on this node
	vector<Agent*> agents;

	// composer that was used to set up simulation on this node
	Composer *comp;

	// the communicator that knows how to send and get messages
	Communicator *communicator;

	// the internal clock on this node
	double currentTime;
};

}


#endif /* PROPAGATOR_CLASS_HH_ */
