/**
 * @file dummywaveaction.hh
 *
 *this file contains the interface of the dummywaveaction
 *
 *@date Oct 14, 2009 last edited 10-15-2009
 *
 *@author garrit jentsch
 */

#ifndef DUMMYWAVEACTION_HH_
#define DUMMYWAVEACTION_HH_

#include "../../agent/action/arnie.hh"

using namespace Hive;

namespace Dummy
{


	/**
	 * @brief this is a dummy implementation of the action class
	 *
	 * a dummywaveaction prints out 'wave' on cerr and adds the number of waves to the agents database
	 * item ...
	 *
	 * @todo error-checking whether the counter exists in the agent
	 *
	 * @author garrit jentsch
	 *
	 * @date Oct 14, 2009 last edited 10-15-2009 by garrit
	 *
	 */
	class DummyWaveAction : public Action {
	public:
		DummyWaveAction();
		DummyWaveAction(int id);

		void fire(Data *d);
	};
}
#endif /* DUMMYWAVEACTION_HH_ */
