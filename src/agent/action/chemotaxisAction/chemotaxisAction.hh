/**
 * @file chemotaxisAction.hh
 *
 * this file contains the declarations of the CELL actions in the chemotaxis simulation.  Other actions
 * are currently defined in the project/chemotaxis directory, but should also be defined here.
 *
 * @todo move all chemotaxis actions to this file
 *
 *
 * @date Oct 22, 2009  Last edited: Oct 23, 2009 Michael and Garrit
 *
 * @author Michael Sneddon
 */

#ifndef CHEMOTAXISACTION_HH_
#define CHEMOTAXISACTION_HH_


#include "../arnie.hh"

using namespace Hive;

namespace Chemotaxis
{

	/**
	 * @brief Cell action to update its current position
	 *  @author Michael Sneddon
	 *  @date Oct 22, 2009
	 */
	class OverwritePositionAction : public Action
	{
		public:
			OverwritePositionAction();
			~OverwritePositionAction();

			/**
			 * executes an action
			 * @param d pointer to data-object containing the argument for the action
			 */
			void fire(Data *d);

		private:
	};


	/**
	 * @brief Cell action to update its internal memory of external chemical concentrations
	 *  @author Michael Sneddon
	 *  @date Oct 22, 2009
	 */
	class UpdateSeenChemicalConcentrationAction : public Action
	{
		public:
			UpdateSeenChemicalConcentrationAction();
			~UpdateSeenChemicalConcentrationAction();

			/**
			 * executes an action
			 * @param d pointer to data-object containing the argument for the action
			 */
			void fire(Data *d);

		private:
	};

	/**
	 * @brief Cell action to update its internal memory of the motor conformation
	 *  @author Michael Sneddon
	 *  @date Oct 22, 2009
	 */
	class UpdateMotorConformationAction : public Action
	{
		public:
			UpdateMotorConformationAction();
			~UpdateMotorConformationAction();

			/**
			 * executes an action
			 * @param d pointer to data-object containing the argument for the action
			 */
			void fire(Data *d);

		private:
	};


}


#endif /* CHEMOTAXISACTION_HH_ */
