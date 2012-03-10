/**
 * @file chemotaxisMessageGenerator.hh
 *
 *	The header file contains declarations of all message generators used
 *	in the chemotaxis system example.
 *
 * @date Oct 21, 2009   last edited: Oct 23, 2009 by Garrit and Michael
 *
 * @author Michael Sneddon
 */

#ifndef CHEMOTAXISMESSAGEGENERATOR_HH_
#define CHEMOTAXISMESSAGEGENERATOR_HH_


#include "../messagegenerator.hh"
#include "../../../projects/chemotaxisAgent/diffusion_data.hh"

using namespace Hive;

namespace Chemotaxis
{

	/**
	 * @brief simple static class to keep track of ActionIDs in the Chemotaxis system
	 *  @author Michael Sneddon
	 *  @date Oct 21, 2009 last edited: Oct 23, 2009 by Garrit and Michael
	 */
	class ChemotaxisActionIDs
	{
		public:
			static const int UPDATE_CELL_VOXEL_POSITION_ACTION_ID;
			static const int UPDATE_CHEMICAL_CONCENTRATION_ACTION_ID;
			static const int OVERWRITE_POSITION_ACTION_ID;
			static const int UPDATE_SEEN_CHEMICAL_CONCENTRATION_ACTION_ID;
			static const int UPDATE_MOTOR_CONFORMATION_ACTION_ID;
			static const int UPDATE_LIGAND_CONCENTRATION_ACTION_ID;
	};


	/**
	 * @brief MessageGenerator to allow cells to tell the World agent where it is
	 *  @author Michael Sneddon
	 *  @date Oct 21, 2009 last edited: Oct 23, 2009 by Garrit and Michael
	 */
	class CellNotifyPosToWorldMessageGenerator : public MessageGenerator{
		public:

			CellNotifyPosToWorldMessageGenerator(Agent *ag);
			~CellNotifyPosToWorldMessageGenerator();
			void placeMessage(int destID);

		private:
			Database *db;

	};

		/**
		 * @brief MessageGenerator to allow cells to tell the World agent what chemicals it consumed or secreted
		 *  @author Michael Sneddon
		 *  @date Oct 21, 2009 last edited: Oct 23, 2009 by Garrit and Michael
		 */
		class CellNotifyDeltaChemicalToWorldMessageGenerator : public MessageGenerator{
			public:
				CellNotifyDeltaChemicalToWorldMessageGenerator(Agent *ag);

				/// destructor
				~CellNotifyDeltaChemicalToWorldMessageGenerator();

				/**
				 * generate message and puts it into the outbox of the source agent
				 */
				void placeMessage(int destID);

			private:
				Database *db;
		};


		/**
		 * @brief MessageGenerator to allow cells to tell NFsim the current ligand concentration
		 *  @author Michael Sneddon
		 *  @date Oct 21, 2009 last edited: Oct 23, 2009 by Garrit and Michael
		 */
		class CellNotifyLigandConcToNFsimMessageGenerator : public MessageGenerator {
			public:
				CellNotifyLigandConcToNFsimMessageGenerator(Agent *ag);

				/// destructor
				~CellNotifyLigandConcToNFsimMessageGenerator();

				/**
				 * generate message and puts it into the outbox of the source agent
				 */
				void placeMessage(int destID);

			private:
				Database *db;
		};


		/**
		 * @brief MessageGenerator to allow the World to overwrite the cells position, for instance, if the cell hits a wall
		 *  @author Michael Sneddon
		 *  @date Oct 21, 2009 last edited: Oct 23, 2009 by Garrit and Michael
		 */
		class WorldNotifyPosToCellMessageGenerator : public MessageGenerator {
			public:
				WorldNotifyPosToCellMessageGenerator(Agent *ag);
				~WorldNotifyPosToCellMessageGenerator();
				void placeMessage(int destID);

			private:
				Database *db;
		};


		/**
		 * @brief MessageGenerator to allow the World to notify the cell of its local chemical environment
		 *  @author Michael Sneddon
		 *  @date Oct 21, 2009 last edited: Oct 23, 2009 by Garrit and Michael
		 */
		class WorldNotifyChemicalConcentrationToCellMessageGenerator : public MessageGenerator {
			public:
				WorldNotifyChemicalConcentrationToCellMessageGenerator(Agent *ag);

				~WorldNotifyChemicalConcentrationToCellMessageGenerator();

				void placeMessage(int destID);

			private:
				Database *db;
		};

		/**
		 * @brief MessageGenerator to allow NFsim to notify the cell of its motor conformational state
		 *  @author Michael Sneddon
		 *  @date Oct 21, 2009 last edited: Oct 23, 2009 by Garrit and Michael
		 */
		class NFsimNotifyMotorConfToCellMessageGenerator : public MessageGenerator {
			public:
				NFsimNotifyMotorConfToCellMessageGenerator(Agent *ag);

				~NFsimNotifyMotorConfToCellMessageGenerator();

				void placeMessage(int destID);

		private:
			Database *db;
	};

}




#endif /* CHEMOTAXISMESSAGEGENERATOR_HH_ */
