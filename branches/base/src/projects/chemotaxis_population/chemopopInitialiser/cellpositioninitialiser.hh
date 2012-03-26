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
#include <sstream>
#include <string>

#include "../../../core/agent/agent.hh"
#include "../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../core/agent/data/template/tdata.hh"
#include "../../../core/util/util.hh"
#include "../../../core/exception/exception.hh"

#include "../chemopopData/grid/grid.hh"
#include "../chemopopData/historyCollector.hh"

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
		/// constructor
		CellPositionInitialiser();

		/// destructor
		~CellPositionInitialiser();

		void initAllInSamePosition(Agent *ag, double x, double y, double z, int num);

		void initUniformDistAlongXAxis(Agent *ag, double minX, double maxX, int num);

		void initRandomlyAroundPosition(Agent *ag, double x, double y, double z, double radius, int num);

		void initRandomlyInCube(Agent *ag, int num);

		void initFromFile(Agent *ag, ifstream& input);

		void initFromFileForMicroFluidic(Agent *ag, ifstream& input, ifstream& geometry);

		bool getIsCubeGrid() { return isCubeGrid; };

		void setIsCubeGrid(bool b) { this->isCubeGrid = b; };

		void setTumbleTriggeredAverageParameters(
				    bool b,
				    double offset,
					double start,
					double end) {
			this->generate_traj_before_tumble_output = b;
			this->generateTrajBeforeTumbleTimeOFFSET=offset;
			this->generateTrajBeforeTumbleTimeSTART=start;
			this->generateTrajBeforeTumbleTimeEND=end;
		};

	private:
		// for initialising the direction and up vecs as well as the cellspeed vector
		// this should also do the initialisation of all vectors of which the length has to be equal to
		// number of cells in the system.
		void doTheRest(Agent *ag);

		int getQuadrant(Coordinate& c);

		// for indicating whether the grid is
		bool isCubeGrid;

		/// cell number ... is it needed?
		int  cellnumber;


		// parameters for tumble triggered averages
		bool generate_traj_before_tumble_output;
		double generateTrajBeforeTumbleTimeOFFSET;
		double generateTrajBeforeTumbleTimeSTART;
		double generateTrajBeforeTumbleTimeEND;


	};
}

#endif /* CELLPOSITIONINITIALISER_HH_ */
