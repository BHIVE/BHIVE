/*
 * chemopopMessageGenerator.hh
 *
 *  Created on: Apr 16, 2010
 *      Author: jentsch
 */

#ifndef CHEMOPOPMESSAGEGENERATOR_HH_
#define CHEMOPOPMESSAGEGENERATOR_HH_

#include "../messagegenerator.hh"
#include "../../data/template/tdata.hh"
#include "../../data/primitive/primitiveData.hh"
#include "../../data/mathfunction/mathfunctiondata.hh"
#include "../../data/mapdata/mapintintdata.hh"
#include "../../data/grid/grid.hh"
#include "../../agentFactory/agent_factory.hh"
//#include "../../../simulators/simulator.hh"



using namespace std;
using namespace Hive;


namespace ChemoPop {
	class ChemoPopActionIDs {
	public:
		// world actions
		static const int UPDATE_WORLD_SEEN_CELLS_SWIMMING_STATE_ACTION_ID;
		static const int UPDATE_WORLD_SEEN_CELL_SPEED_ACTION_ID;
		static const int UPDATE_WORLD_BACKGROUND_CONCENTRATION_ACTION_ID;
		static const int UPDATE_WORLD_SEEN_BLINDAGENTS_ANGLES_ACTION_ID;
		static const int UPDATE_WORLD_DESIRED_CONSUMPTION_ACTION_ID;
		static const int UPDATE_WORLD_BLIND_AGENT_DEATH_ACTION_ID;
		static const int UPDATE_WORLD_BLIND_AGENT_BIRTH_ACTION_ID;

		// cell actions
		static const int UPDATE_CELL_SEEN_LIGAND_ACTION_ID;
		static const int UPDATE_CELL_ACCUMULATED_LIGAND_ACTION_ID;

		// special agent action
		static const int SPECIAL_AGENT_RECEIVE_CELL_POSITIONS_ACTION_ID;
		static const int SPECIAL_AGENT_ACCUMULATE_CELL_DATA_ACTION_ID;
		static const int SPECIAL_AGENT_ACCUMULATE_BLINDAGENT_DATA_ACTION_ID;
		static const int SPECIAL_AGENT_UPDATE_BLIND_AGENT_COUNT_ACTION_ID;
		static const int SPECIAL_AGENT_OUTPUT_GRID_ACTION_ID;
		static const int SPECIAL_AGENT_OUTPUT_ANALYSED_CELL_POSITIONS_DATA_ACTION_ID;
	};

	// message with which the world tells a cell agent about the ligands in the cell's local environment
	class WorldNotifyLigandToCellMessageGenerator : public MessageGenerator {
	public:
		WorldNotifyLigandToCellMessageGenerator(Agent* ag);

		~WorldNotifyLigandToCellMessageGenerator();

		void placeMessage(int destID);

	private:
		Database *db;
		TVectorData<TVectorData<double>* > *cell_positions;
		bool isGridEnv;
		Grid *environGrid;
		TVectorData<MathFunctionData* > *environMath;
		TVectorData<double>* birthtimes;
		DoubleData *t;
		MapIntIntData *local_index_map;
	};

	// message with which the world notifies the cell how much nutrient it consumed in the last step
	class WorldNotifyCellAboutConsumedNutrientMessageGenerator : public MessageGenerator {
	public:
		WorldNotifyCellAboutConsumedNutrientMessageGenerator(Agent *ag);

		~WorldNotifyCellAboutConsumedNutrientMessageGenerator();

		void placeMessage(int destID);

	private:
		Database *db;
		TVectorData<double> *dcc;
		MapIntIntData *local_index_map;

	};

	// message with which the world tells the special agent about the positions of the cells for output
	class WorldTellSpecialAgentCellPositionsMessageGenerator : public MessageGenerator {
	public:
		WorldTellSpecialAgentCellPositionsMessageGenerator(Agent *ag);

		~WorldTellSpecialAgentCellPositionsMessageGenerator();

		bool isMessageForSpecialAgent() { return true; };

		void placeMessage(int destID);

	private:
		Database *db;
		double nextOutputTime;
		DoubleData *t ;
		DoubleData *ov;
		TVectorData<TVectorData<double>* > *positions;
		MapIntIntData *index_agentid_map;

	};


	// message generator in which the world already analyses the cell positions and sends the information to the output agent
	class WorldTellSpecialAgentAnalysedCellPositionsDataMessageGenerator : public MessageGenerator {
	public:
		WorldTellSpecialAgentAnalysedCellPositionsDataMessageGenerator(Agent *ag);

		~WorldTellSpecialAgentAnalysedCellPositionsDataMessageGenerator();

		bool isMessageForSpecialAgent() { return true; };

		void placeMessage(int destID);

	private:
		Database *db;
		double nextOutputTime;
		DoubleData *t;
		DoubleData *ov;
		TVectorData<TVectorData<double>* >* positions;
		MapIntIntData *index_agentid_map;
	};

	// message to send a grid to the special agent for output needs to be implemented!
	class WorldTellSpecialAgentAboutGridMessageGenerator : public MessageGenerator {
	public:
		WorldTellSpecialAgentAboutGridMessageGenerator(Agent *ag);

		~WorldTellSpecialAgentAboutGridMessageGenerator();

		bool isMessageForSpecialAgent() { return true; };

		void placeMessage(int destID);

	private:
		Database *db;
		double nextOutputTime;
		DoubleData *t;
		DoubleData *ov;
		Grid *g;
	};

	// message with which the cell agent tells the world agent about its swimming state
	class CellNotifyWorldAboutSwimmingStateMessageGenerator : public MessageGenerator {
	public:
		CellNotifyWorldAboutSwimmingStateMessageGenerator(Agent *ag);

		~CellNotifyWorldAboutSwimmingStateMessageGenerator();

		void placeMessage(int destID);

	private:
		Database *db;
		IntegerData *swimmingstate;
	};

	// message with which the cell agent tells the world agent about a change in its speed
	class CellNotifyWorldAboutNewSpeedMessageGenerator : public MessageGenerator {
		CellNotifyWorldAboutNewSpeedMessageGenerator(Agent *ag);

		~CellNotifyWorldAboutNewSpeedMessageGenerator();

		void placeMessage(int destID);

	private:
		Database *db;
	};

	// message with which a cell tells its internal state to the special agent for output
	class CellRevealItselfToSpecialAgentMessageGenerator : public MessageGenerator {
	public:
		CellRevealItselfToSpecialAgentMessageGenerator(Agent *ag);

		~CellRevealItselfToSpecialAgentMessageGenerator();

		bool isMessageForSpecialAgent() { return true; };

		void placeMessage(int destID);

	private:
		Database *db;
		double nextOutputTime;
		DoubleData *t;
		DoubleData *ov;
		TVectorData<double> *cheyp;
		TVectorData<double> *methLevel;
		IntegerData *swimmingstate;
		TVectorData<double> *ligand;
		TVectorData<double> *accligand;
		TVectorData<int> *motorStates;


	};

	// message with which a cell tells the world agent how much nutrient it would like to consume
	class CellTellWorldAboutDesiredNutrientConsumptionMessageGenerator : public MessageGenerator {
	public:
		CellTellWorldAboutDesiredNutrientConsumptionMessageGenerator(Agent *ag);

		~CellTellWorldAboutDesiredNutrientConsumptionMessageGenerator();

		bool isMessageForSpecialAgent() { return false; };

		void placeMessage(int destID);

	private:
		Database *db;
		TVectorData<double> *app;

	};

	// message with which a blind agent tells the world about an update in its angle
	class BlindAgentNotifyWorldAboutAngleAndVelocityMessageGenerator : public MessageGenerator {
	public:
		BlindAgentNotifyWorldAboutAngleAndVelocityMessageGenerator(Agent *ag);

		~BlindAgentNotifyWorldAboutAngleAndVelocityMessageGenerator();

		void placeMessage(int destID);

	private:
		Database *db;

		DoubleData *ang;
		DoubleData *vel;
	};



	// message with which a blind agent tells the world that it has expired
	class BlindAgentNotifyWorldThatItIsDead : public MessageGenerator {
	public:
		BlindAgentNotifyWorldThatItIsDead(Agent *ag);

		~BlindAgentNotifyWorldThatItIsDead();

		void placeMessage(int destID);

	private:
		Database *db;
		BoolData *deathFlag;
	};



	// message with which a blind agent tells the world that it has produced offspring
	class BlindAgentNotifyWorldThatNewAgentIsBorn : public MessageGenerator {
	public:
		BlindAgentNotifyWorldThatNewAgentIsBorn(Agent *ag);

		~BlindAgentNotifyWorldThatNewAgentIsBorn();

		void placeMessage(int destID);

	private:
		Database *db;
		BoolData *birthFlag;
		BoolData *deathFlag;
	};




	// message with which a blind agent reveals itself to the blind agent for output
	class BlindAgentRevealItselfToSpecialAgentMessageGenerator : public MessageGenerator {
	public:
		BlindAgentRevealItselfToSpecialAgentMessageGenerator(Agent *ag);

		~BlindAgentRevealItselfToSpecialAgentMessageGenerator();

		bool isMessageForSpecialAgent() { return true; };

		void placeMessage(int destID);

	private:
		Database *db;
		DoubleData *nextOutputTime;

		DoubleData *t;
		DoubleData *ov;

		string s;
		int number_of_letters;

		DoubleData *dist_to_travel;
		DoubleData *essence;
		DoubleData *km;
		DoubleData *kcat;
		DoubleData *yield;
		DoubleData *velocity;
		DoubleData *rho;
		DoubleData *stepLengthDist;
		IntegerData *generation;

	};
}

#endif /* CHEMOPOPMESSAGEGENERATOR_HH_ */
