/*
 * chemotaxisworldactions.hh
 *
 *this file contains the interfaces to the actions that can be called for the world agent
 *
 *
 *@date Oct 21th, 2009 last edited 10-21 by garrit
 *@author garrit jentsch
 */
#ifndef CHEMOTAXISWORLDACTIONS_HH_
#define CHEMOTAXISWORLDACTIONS_HH_

#include "../../agent/action/arnie.hh"
#include "../../agent/data/primitive/primitiveData.hh"
#include "diffusion_data.hh"

using namespace Hive;
using namespace std;

namespace Chemotaxis {
	/**
	 * @brief this action updates the voxel-positions of the cells in the world's cells_voxel_positions
	 *
	 * @author Garrit Jentsch
	 *
	 * @date Oct 21th, 2009  last edited 10-21-2009 by garrit
	 *
	 */
	class UpdateCellVoxelPositionAction : public Action {
	public:
		UpdateCellVoxelPositionAction();
		UpdateCellVoxelPositionAction(int id);

		~UpdateCellVoxelPositionAction() {};

		void fire(Data *d);
	};

	class UpdateChemicalConcentrationAction:public Action {
	public:
		UpdateChemicalConcentrationAction();
		UpdateChemicalConcentrationAction(int id);

		~UpdateChemicalConcentrationAction() {};

		void fire(Data *d);
	};
}

#endif /* CHEMOTAXISWORLDACTIONS_HH_ */
