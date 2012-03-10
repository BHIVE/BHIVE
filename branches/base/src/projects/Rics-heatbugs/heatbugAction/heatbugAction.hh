/*
 * heatbugAction.hh
 *
 *  Created on: Jan 4, 2011
 *      Author: jentsch
 */

#ifndef HEATBUGACTION_HH_
#define HEATBUGACTION_HH_

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>

#include "../../../core/agent/action/arnie.hh"
#include "../heatbugMessageGenerator/heatbugMessageGenerator.hh"
#include "../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../core/agent/data/template/tdata.hh"
#include "../../../core/agent/data/mapdata/mapintintdata.hh"
#include "../../../core/agent/data/mathfunction/mathfunctiondata.hh"
#include "../../../core/exception/exception.hh"
#include "../../../core/agent/special_agent.hh"
#include "../../../core/util/util.hh"

using namespace std;
using namespace Hive;

namespace Heatbug {

	class WorldUpdateDesiredMovementOfBugAction : public Action {
	public:
		WorldUpdateDesiredMovementOfBugAction();
		~WorldUpdateDesiredMovementOfBugAction();

		void fire(Data *d);
		void prepare();

	private:
		TVectorData<TVectorData<TVectorData<IntegerData>* >* > *desired_bug_grid;
		TVectorData<TVectorData<IntegerData>* > *desired_bug_positions;
		TVectorData<TVectorData<IntegerData>* > *bug_positions;
		int gridsize;
	};

	class WorldUpdateTemperatureProducedByBugAction : public Action {
	public:
		WorldUpdateTemperatureProducedByBugAction();
		~WorldUpdateTemperatureProducedByBugAction();

		void fire(Data *d);
		void prepare();

	private:
		TVectorData<TVectorData<DoubleData>* > *temperature_grid;
		TVectorData<TVectorData<IntegerData>* > *bug_positions;
		double dt;
	};

	class BugUpdateSeenTemperatureAction : public Action {
	public:
		BugUpdateSeenTemperatureAction();
		~BugUpdateSeenTemperatureAction();

		void fire(Data *d);
		void prepare();

	private:
		TVectorData<DoubleData> *local_temperature_around_me;
	};

	class SpecialAgentReceiveBugPositionsAction : public Action {
	public:
		SpecialAgentReceiveBugPositionsAction();
		~SpecialAgentReceiveBugPositionsAction();

		void fire(Data *d);
		void prepare();
		void prepareOutputfile(string, bool);

	private:
		ofstream *out;
		bool isbinary;
	};

	class SpecialAgentReceiveTemperatureFieldAction : public Action {
	public:
		SpecialAgentReceiveTemperatureFieldAction();
		~SpecialAgentReceiveTemperatureFieldAction();

		void fire(Data *d);
		void prepare();
		void prepareOutputfile(string, bool);

	private:
		ofstream *out;
		bool isbinary;
	};

	class SpecialAgentUpdateBugHappinessAction : public Action {
	public:
		SpecialAgentUpdateBugHappinessAction();
		~SpecialAgentUpdateBugHappinessAction();

		void fire(Data *d);
		void prepare();
		void prepareOutputfile(string, bool);

	private:
		ofstream *out;
		bool      isbinary;
		int       number_received;
		int       number_bugs;
		double    output_interval;
		double    acc_happiness;
		int       number_outputs;
	};

}

#endif /* HEATBUGACTION_HH_ */
