/*
 * heatbugMessageGenerator.hh
 *
 *  Created on: Jan 3, 2011
 *      Author: jentsch
 */

#ifndef HEATBUGMESSAGEGENERATOR_HH_
#define HEATBUGMESSAGEGENERATOR_HH_

#include "../../../core/agent/messageGenerator/messagegenerator.hh"
#include "../../../core/agent/data/template/tdata.hh"
#include "../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../core/agent/data/mathfunction/mathfunctiondata.hh"
#include "../../../core/agent/data/mapdata/mapintintdata.hh"
#include "../../../core/agent/agentFactory/agent_factory.hh"

using namespace std;
using namespace Hive;

namespace Heatbug {
	class HeatbugActionIDs {
	public:
		static const int WORLD_UPDATE_DESIRED_MOVEMENT_OF_BUG_ACTION_ID;
		static const int WORLD_UPDATE_TEMPERATURE_PRODUCED_BY_BUG_ACTION_ID;

		static const int BUG_UPDATE_SEEN_TEMPERATURE_ACTION_ID;

		static const int SPECIAL_AGENT_RECEIVE_BUG_POSITIONS_ACTION_ID;
		static const int SPECIAL_AGENT_RECEIVE_TEMPERATURE_FIELD_ACTION_ID;
		static const int SPECIAL_AGENT_UPDATE_BUG_HAPPINESS_ACTION_ID;
	};

	// message generator by which the world tells each bug what the temperature in its vicinity is.
	class WorldNotifyBugAboutLocalTemperatureMessageGenerator : public MessageGenerator {
	public:
		WorldNotifyBugAboutLocalTemperatureMessageGenerator(Agent *ag);
		~WorldNotifyBugAboutLocalTemperatureMessageGenerator();

		void placeMessage(int destID);
	private:
		Database *db;
		TVectorData<TVectorData<DoubleData>* > *temperature_grid;
		TVectorData<TVectorData<IntegerData>* > *bug_positions;
		int grid_size;
	};

	// message generator by which the bug tells the world into which direction it wants to move
	class BugNotifyWorldAboutDesiredMovementMessageGenerator : public MessageGenerator {
	public:
		BugNotifyWorldAboutDesiredMovementMessageGenerator(Agent *ag);
		~BugNotifyWorldAboutDesiredMovementMessageGenerator();

		void placeMessage(int destID);
	private:
		Database *db;
		IntegerData *move_direction;
	};

	// message generator by which the bug tells the world about the temperature it has produced
	class BugNotifyWorldAboutProducedHeatMessageGenerator : public MessageGenerator {
	public:
		BugNotifyWorldAboutProducedHeatMessageGenerator(Agent *ag);
		~BugNotifyWorldAboutProducedHeatMessageGenerator();

		void placeMessage(int destID);

	private:
		Database *db;
		DoubleData *heat_rate;
	};

	class WorldTellSpecialAgentAboutBugPositionsMessageGenerator : public MessageGenerator {
	public:
		WorldTellSpecialAgentAboutBugPositionsMessageGenerator(Agent *ag);
		~WorldTellSpecialAgentAboutBugPositionsMessageGenerator();

		bool isMessageForSpecialAgent() { return true; } ;

		void placeMessage(int destID);

	private:
		Database *db;
		TVectorData<TVectorData<IntegerData>* > *bug_grid;

		DoubleData *time;
		DoubleData *noti;
	};

	class WorldTellSpecialAgentAboutTemperatureGridMessageGenerator : public MessageGenerator {
	public:
		WorldTellSpecialAgentAboutTemperatureGridMessageGenerator(Agent *ag);
		~WorldTellSpecialAgentAboutTemperatureGridMessageGenerator();

		bool isMessageForSpecialAgent() { return true; };

		void placeMessage(int dest);

	private:
		Database *db;
		TVectorData<TVectorData<DoubleData>* > *temperature_grid;

		DoubleData *time;
		DoubleData *noti;
		DoubleData *oi;
	};

	class BugTellSpecialAgentAboutHappinessMessageGenerator : public MessageGenerator {
	public:
		BugTellSpecialAgentAboutHappinessMessageGenerator(Agent *ag);
		~BugTellSpecialAgentAboutHappinessMessageGenerator();

		bool isMessageForSpecialAgent() { return true; };

		void placeMessage(int dest);

	private:
		Database *db;
		DoubleData *preferred_temperature;
		TVectorData<DoubleData> *local_temperature;
		DoubleData *time;
		DoubleData *noti;
		DoubleData *oi;
	};

}


#endif /* HEATBUGMESSAGEGENERATOR_HH_ */
