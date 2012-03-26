/**
 * @file transwellMessageGenerator.hh
 *
 * this header file contains declarations of all message generators that are used
 * in the transwell example.
 *
 * @date Jan 13, 2010 last edited 1-13-10 by garrit
 *
 * @author garrit jentsch
 */

#ifndef TRANSWELLMESSAGEGENERATOR_HH_
#define TRANSWELLMESSAGEGENERATOR_HH_

#include "../../../core/agent/messageGenerator/messagegenerator.hh"
#include "../../../core/agent/data/template/tdata.hh"
#include "../../../core/agent/data/primitive/primitiveData.hh"

#include "../data/grid/grid.hh"
#include "../data/grid/nvoxel.hh"

using namespace Hive;

namespace Transwell {

	/**
	 * @brief simple static class to keep track of actionids in the transwell system
	 * @author garrit jentsch
	 * @date Jan 13, 2010
	 */
	class TranswellActionIDs {
	public:
		static const int UPDATE_CELL_SEEN_IL2_ACTION_ID;
		static const int UPDATE_NUCLEUS_SEEN_PSTAT5_ACTION_ID;
		static const int UPDATE_CELL_SEEN_PARTICLES_ACTION_ID;
		static const int UPDATE_WORLD_SEEN_IL2_ACTION_ID;
		static const int INIT_WORLD_CELL_POSITION_ACTION_ID;
		static const int INIT_WORLD_TREG_POSITION_ACTION_ID;
		static const int SPECIAL_AGENT_ACCUMULATE_CELL_DATA_ACTION_ID;
		static const int SPECIAL_AGENT_OUTPUT_GLOBAL_CONCENTRATIONS_ID;
		static const int SPECIAL_AGENT_OUTPUT_IL2_PROFILE_ID;
		static const int SPECIAL_AGENT_ACCUMULATE_TREG_DATA_ACTION_ID;
	};

	/**
	 * @brief messagegenerator to allow the world to tell the cells what il2 concentration they see.
	 * @author garrit jentsch
	 * @date Jan 13, 2010
	 */
	class WorldNotifyIL2ToCellMessageGenerator : public MessageGenerator {
	public:
		/// constructor
		WorldNotifyIL2ToCellMessageGenerator(Agent *ag);

		/// destructor
		~WorldNotifyIL2ToCellMessageGenerator();

		/**
		 * generate message and puts it into the outbox of the source agent
		 */
		void placeMessage(int destID);

	private:
		Database *db;
	};

	/**
	 * @brief messagegenerator for the world to tell the special agent about the il2 particles in the small and large well
	 * @author garrit jentsch
	 * @date Feb 15, 2010
	 */
	class WorldTellIL2ToSpecialAgentMessageGenerator : public MessageGenerator {
	public:
		/// constructor
		WorldTellIL2ToSpecialAgentMessageGenerator(Agent* ag);

		/// destructor
		~WorldTellIL2ToSpecialAgentMessageGenerator();

		/// make sure that this is a message-generator to send messages to the special-agent
		bool isMessageForSpecialAgent() { return true; }

		/// generate message and place it into the outbox of the source agent
		void placeMessage(int destID);

	private:
		Database *db;
	};


	class WorldTellIL2ProfileToSpecialAgentMessageGenerator : public MessageGenerator {
	public:
		/// constructor
		WorldTellIL2ProfileToSpecialAgentMessageGenerator(Agent *ag);

		/// destructor
		~WorldTellIL2ProfileToSpecialAgentMessageGenerator();

		bool isMessageForSpecialAgent() { return true; };

		void placeMessage(int destID);

	private:
		Database *db;
	};

	/**
	 * @brief messagegenerator to allow the cell to tell the nucleus what the pstat5 concentration is.
	 * @author garrit jentsch
	 * @date Jan 13, 2010
	 */
	class CellNotifyPSTAT5ToNucleusMessageGenerator : public MessageGenerator {
	public:
		/// constructor
		CellNotifyPSTAT5ToNucleusMessageGenerator(Agent *ag);

		/// destructor
		~CellNotifyPSTAT5ToNucleusMessageGenerator();

		/// generate message and place it into the outbox of the source agent
		void placeMessage(int destID);

	private:
		Database *db;
	};

	/**
	 * @brief messagegenerator to allow the nucleus to tell the cell about its particle numbers.
	 * @author garrit jentsch
	 * @date Jan 13, 2010
	 */
	class NucleusNotifyParticleNumbersToCellMessageGenerator : public MessageGenerator {
	public:
		/// constructor
		NucleusNotifyParticleNumbersToCellMessageGenerator(Agent *ag);

		/// destructor
		~NucleusNotifyParticleNumbersToCellMessageGenerator();

		/// generate message and place it into the outbox
		void placeMessage(int destID);

	private:
		Database *db;
	};

	/**
	 * @brief messagegenerator to allow the cell to tell the world about the il2 concentration
	 * @author garrit jentsch
	 * @date Jan 13, 2010
	 */
	class CellNotifyIL2ToWorldMessageGenerator : public MessageGenerator {
	public:
		/// constructor
		CellNotifyIL2ToWorldMessageGenerator(Agent *ag);

		/// destructor
		~CellNotifyIL2ToWorldMessageGenerator();

		/// generate message and place it into the outbox
		void placeMessage(int destID);

	private:
		Database *db;
	};

	/**
	 * @brief messagegenerator that allows a cell to tell the world its position for initialisation of the world
	 * @author garrit jentsch
	 * @date Jan 13, 2010
	 */
	class CellNotifyWorldAboutPositionForInitMessageGenerator : public MessageGenerator {
	public:
		/// constructor
		CellNotifyWorldAboutPositionForInitMessageGenerator(Agent *ag);

		/// destructor
		~CellNotifyWorldAboutPositionForInitMessageGenerator();

		/// generate message and place it into the outbox
		void placeMessage(int destID);

	private:
		Database *db;
	};


	/**
	 * @brief messagegenerator that allows a TREG cell to tekk the world its position for initialisation
	 * @author garrit jentsch
	 * @date march 15, 2010
	 */
	class TregNotifyWorldAboutPositionForInitMessageGenerator : public MessageGenerator {
	public:
		/// constructor
		TregNotifyWorldAboutPositionForInitMessageGenerator(Agent *ag);

		/// destructor
		~TregNotifyWorldAboutPositionForInitMessageGenerator();

		/// generate message and place it onto outbox
		void placeMessage(int destID);

	private:
		Database *db;
	};


	/**
	 * @brief messagegenerator for the cell to send all its data to the output special agent
	 * @author garrit jentsch
	 * @date jan 19th, 2010
	 */
	class CellRevealItselfToSpecialAgentMessageGenerator : public MessageGenerator {
	public:
		/// constructor
		CellRevealItselfToSpecialAgentMessageGenerator(Agent *ag);

		/// make sure that this is a message-generator to send messages to the special-agent
		bool isMessageForSpecialAgent() { return true; };

		/// destructor
		~CellRevealItselfToSpecialAgentMessageGenerator();

		/// generate message and place it on to the outbox
		void placeMessage(int destID);

	private:
		Database *db;
	};

	/**
	 * @brief messagegenerator for the TREG cell to send all its data to the output special agent
	 * @author garrit jentsch
	 * @date mar 15th, 2010
	 */
	class TregRevealItselfToSpecialAgentMessageGenerator : public MessageGenerator {
	public:
		/// constructor
		TregRevealItselfToSpecialAgentMessageGenerator(Agent *ag);

		/// destructor
		~TregRevealItselfToSpecialAgentMessageGenerator();

		bool isMessageForSpecialAgent() { return true; };

		void placeMessage(int destID);

	private:
		Database *db;
	};
}

#endif /* TRANSWELLMESSAGEGENERATOR_HH_ */
