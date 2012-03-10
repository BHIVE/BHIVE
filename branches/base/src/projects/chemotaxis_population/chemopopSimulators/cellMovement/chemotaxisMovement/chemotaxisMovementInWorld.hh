/*
 * chemotaxisMovementInWorld.hh
 *
 *  Created on: Apr 19, 2010
 *      Author: jentsch
 */

#ifndef CHEMOTAXISMOVEMENTINWORLD_HH_
#define CHEMOTAXISMOVEMENTINWORLD_HH_

#include <cmath>
#include <fstream>

#include "../../../../../core/agent/data/data.hh"
#include "../../../../../core/agent/data/template/tdata.hh"
#include "../../../../../core/agent/data/primitive/primitiveData.hh"

#include "../../../../../core/agent/data/boundary/boundarydata.hh"

#include "../../../chemopopData/grid/grid.hh"
#include "../../../chemopopData/grid/nvoxel.hh"
#include "../../../../../core/simulators/simulator.hh"
#include "../../../../../core/util/util.hh"

#include "../../../chemopopInitialiser/cellparameterinitializer.hh"
#include "../../../chemopopData/historyCollector.hh"


using namespace std;
using namespace Hive;

namespace ChemoPop {

	/*
	 *@brief this is michael's chemotaxismovementsimulator that has been adapted such that
	 *       it runs on the world level instead of the cell level
	 */
	class ChemotaxisMovementSimulatorInWorld : public Simulator {
	public:
		ChemotaxisMovementSimulatorInWorld();
		virtual ~ChemotaxisMovementSimulatorInWorld();

		void setWorldAgent(Agent *ag);

		void setGrid(string gridname);

		void step(double t); // t is the timestep that we need to
		void prepare();
		void synchroniseWithDatabase();

		void setGenerateRunTumbleOutput(bool b, string filename);
		void setGenerateXDirTrajBeforeTumbleOutput(bool b);

	protected:
		void initialise();

	private:
		// not quite sure what these do, i assume
		void changeDirRandom(int i);
		void changeDirDistribution(int i);
		void changeDirRotDiffusion(int i);
		void swimToNewPosition(int i); // what are the parameters ?!
		void moveToNewPosition(int i, double distance);
		void ReflectOffWall(int i, int j);
		void SimpleParallelWall(int i, int j);
		void changeDirFromWall(int i, double h, double dist, int wall);





		/// whether we are using grid to determine cell movement
		bool isGrid;
		/// grid of the world agent.
		Grid *g;

		double eqTime;

		double internalTime;

		// here we declare all the pointers to the data-structures of the world that are needed for the simulation
		IntegerData *numberofcells;
		DoubleData  *rotdiffconst;
		DoubleData  *movement_dt;
		BoolData *isIn2D;

		bool tumble_style_use_gamma;

		TVectorData<double> *cellspeeds;
		TVectorData<int>    *last_swimming_states;
		TVectorData<int>    *swimming_states;

		TVectorData<TVectorData<double>* > *cell_positions;
		TVectorData<int> *cell_voxels;
		TVectorData<TVectorData<double>* > *cell_dir_vecs;
		TVectorData<TVectorData<double>* > *cell_up_vecs;
		SimpleBoundaryData * d;

		TVectorData< SimpleBoundaryData * > *boundaries;

		StringData *boundary_behavior;
		DoubleData *wall_rot;

		double rotMat[3][3];
		Util::GammaSampler *gs;


		// For output tabulation / generation
		ofstream *run_tumble_output_stream;
		bool generate_run_tumble_output;
		void recordRunTumble(int cellIndex,int event);


		bool generate_Xdir_output;
		TVectorData <HistoryCollector *>  *hcXDirectionCollectors;
		TVectorData <HistoryAggregator *> *haXDirectionAggregators;


	};



}



#endif /* CHEMOTAXISMOVEMENTINWORLD_HH_ */
