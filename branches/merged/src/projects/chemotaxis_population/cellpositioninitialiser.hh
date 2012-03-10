/*
 * cellpositioninitialiser.hh
 *
 *  Created on: Apr 18, 2010
 *      Author: jentsch
 */

#ifndef CELLPOSITIONINITIALISER_HH_
#define CELLPOSITIONINITIALISER_HH_

#include <fstream>
#include <iostream>

#include "../../agent/agent.hh"
#include "../../agent/data/primitive/primitiveData.hh"
#include "../../agent/data/grid/grid.hh"
#include "../../agent/data/template/tdata.hh"
#include "../../util/util.hh"
#include "../../exception/exception.hh"

using namespace std;
using namespace Hive;


namespace ChemoPop {
	/**
	 * @brief just a class to initialise cell positions
	 *
	 * @author garrit
	 *
	 * @date 4-18-10
	 */
	class CellPositionInitialiser {
	public:
		CellPositionInitialiser();
		~CellPositionInitialiser();
		void initAllInSamePosition(Agent *ag, double x, double y, double z);
		void initRandomlyAroundPosition(Agent *ag, double x, double y, double z, double radius);

		void initRandomlyInCube(Agent *ag);

		void initFromFile(Agent *ag, ifstream& input);

		bool getIsCubeGrid() { return isCubeGrid; };
		void setIsCubeGrid(bool b) { this->isCubeGrid = b; };

	private:
		bool isCubeGrid;
	};
}

#endif /* CELLPOSITIONINITIALISER_HH_ */
