/*
 * nfsimagentfactory.hh
 *
 *  Created on: Mar 15, 2011
 *      Author: jentsch
 */

#ifndef NFSIMAGENTFACTORY_HH_
#define NFSIMAGENTFACTORY_HH_

#include "../../../core/agent/agentFactory/agent_factory.hh"
#include "../../../core/agent/data/data.hh"
#include "../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../core/agent/data/template/tdata.hh"

#include "../../../core/exception/exception.hh"
#include "../../../core/simulators/timekeeper/timekeeper.hh"

#include "../nfsimDataWrapper/nfsimdata.hh"

#include "../nfsimSimulator/NFsim/NFcore/NFcore.hh"
#include "../nfsimSimulator/NFsim/NFinput/NFinput.hh"
#include "../nfsimSimulator/NFsim/NFreactions/NFreactions.hh"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;
using namespace Hive;

namespace nfsimparascan {

	class NFsimAgentFactory : public AgentFactory {
	public:
		NFsimAgentFactory();
		~NFsimAgentFactory();

		Agent *createAgent();
		Agent *createAgent(Action **as);

		Agent *duplicateAgent(Agent *ag);

		void finalise();

		// methods to bring private members to life
		void setXMLFilename(string);
		void setBlockSameComplexBinding(bool b);
		void setTurnOnComplexBookkeeping(bool b);
		void setGlobalMoleculeLimit(int);
		void setVerbose(bool b);

	protected:
		void addActionsToAgentsActionSet(Agent *ag);
		void addSimulatorToAgent(Agent *ag);
		void addMessageGeneratorsToAgent(Agent *ag);

	private:
		// some local variables that are needed to initialise an NFsimSystem.
		string filename;

		bool blockSameComplexBinding;

		bool cb;

		bool turnOnComplexBookkeeping;

		bool verbose;

		int globalMoleculeLimit;

	};
}

#endif /* NFSIMAGENTFACTORY_HH_ */
