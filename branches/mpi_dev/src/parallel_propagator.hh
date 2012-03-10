/*
 * parallel_propagator.hh
 *
 *  Created on: Feb 24, 2010
 *      Author: jentsch
 */

#ifndef PARALLEL_PROPAGATOR_HH_
#define PARALLEL_PROPAGATOR_HH_

#include "propagator_class.hh"

namespace Hive {

class ParallelPropagator : public PropagatorClass {
public:
	ParallelPropagator(Composer *composer);

	~ParallelPropagator();

	void go(double dt, double endTime);

	void equilibrate(double time);

	void setup();

};

}
#endif /* PARALLEL_PROPAGATOR_HH_ */
