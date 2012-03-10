/*
 * blindMovementInWorld.hh
 *
 *  Created on: Jun 16, 2010
 *      Author: msneddon
 */

#ifndef BLINDMOVEMENTINWORLD_HH_
#define BLINDMOVEMENTINWORLD_HH_


#include <cmath>

#include "../../../agent/data/data.hh"
#include "../../../agent/data/grid/grid.hh"
#include "../../../agent/data/template/tdata.hh"
#include "../../../agent/data/primitive/primitiveData.hh"

#include "../../../projects/chemotaxis_population/cellparameterinitializer.hh"

#include "../../simulator.hh"
#include "../../../util/util.hh"

using namespace std;
using namespace Hive;

namespace ChemoPop {

	/**
	 *@brief blind movement simulator that updates positions in the world of a blind agent
	 */
	class BlindMovementSimulatorInWorld : public Simulator {
	public:
		BlindMovementSimulatorInWorld();
		~BlindMovementSimulatorInWorld();

		void setWorldAgent(Agent *ag);

		void step(double t); // t is the timestep that we need to move for
		void prepare();
		void synchroniseWithDatabase();

	protected:
		void initialise();

	private:

		//This will perform these operations for blind cell named i.

		/**
		 * Change our directional and up vectors completely at random for cell i.
		 * This is useful for initialization.
		 * @param i
		 */
		void changeDirUniformRandom(int i);

		/**
		 * Change our directional and up vectors according to the change in angle
		 * provided by the cell (which is set in an action based on a message from
		 * the blind cell)
		 * @param i
		 */
		void changeDirBasedOnNextTurnAngle(int i);


		/**
		 * This function moves the cell at index i to a new position
		 * based on the cells speed
		 * @param i, the index of the blind cell
		 */
		void moveToNewPosition(int i);




		Grid *g;
		BoolData *isGrid;
		TVectorData<int> *cell_voxel_position;
		TVectorData<int> *number_cells_per_voxel;

		double cube_side_length;

		/** equilibration time */
		double eqTime;

		/** internal clock */
		double internalTime;


		// here we declare all the pointers to the data-structures of the world that are needed for the simulation
		IntegerData *numberofcells;

		/** the timestep of movement, overridden here by the step function */
		DoubleData  *movement_dt;


		// This information determines all cells, that is the
		// cell speeds, positions, directions and up direction.  direction
		// and up direction always have to be unit vectors, or else some of
		// the operations will break

		TVectorData<double> *cellspeeds;
		TVectorData<int>    *cell_wants_to_move;
		TVectorData<TVectorData<double>* > *cell_positions;
		TVectorData<TVectorData<double>* > *cell_dir_vecs;
		TVectorData<TVectorData<double>* > *cell_up_vecs;


		// This is the way the cell controls is motion over
		// the next time step.
		TVectorData<double> *nextTurnAngle;

		// rotation matrix, direction, and up vector arrays, created here so we
		// don't have to create a new one locally each time.
		double rotMat[3][3];
		double local_dir[3];
		double local_up[3];


		// For now, there is no additional rotational diffusion term.
		//void changeDirRotDiffusion(int i);
		//DoubleData  *rotdiffconst;
	};

};

#endif /* BLINDMOVEMENTINWORLD_HH_ */
