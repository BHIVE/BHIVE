/*
 * @file dummycomposer.hh
 *
 * this file contains the interface to the dummycomposer.
 *
 * @author Garrit jentsch
 *
 * @date Oct 14th, 2009 last edited 10-15-09 by garrit
 */

#ifndef DUMMYCOMPOSER_HH_
#define DUMMYCOMPOSER_HH_

#include "../../composer/composer.hh"
#include "dummyagentfactory.hh"
#include "dummywaveaction.hh"
#include "../../agent/data/primitive/primitiveData.hh"
#include "../../communicator/serial/serialcommunicator.hh"


using namespace Hive;

namespace Dummy
{

	/*
	 * @brief the dummycomposer implements a composer
	 *
	 * this is a minimal implementation of the composer class. it does not really do anything.
	 *
	 *@author Garrit Jentsch
	 *
	 *@date Oct 14th, 2009 last edited 10-15-09 by garrit
	 */
	class DummyComposer : public Composer {
		public:

			DummyComposer();

			~DummyComposer();

			void setupSimulation();

		protected:
			void initializeAgentFactories();

			void setupAgentHierarchy();

			Agent* getTopLevelAgent();

			Agent* createAgent(AgentFactory&);

		private:
	};
}
#endif /* DUMMYCOMPOSER_HH_ */
