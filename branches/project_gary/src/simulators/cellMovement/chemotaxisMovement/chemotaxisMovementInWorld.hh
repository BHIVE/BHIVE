/*
 * chemotaxisMovementInWorld.hh
 *
 *  Created on: Apr 19, 2010
 *      Author: jentsch
 */

#ifndef CHEMOTAXISMOVEMENTINWORLD_HH_
#define CHEMOTAXISMOVEMENTINWORLD_HH_

#include <cmath>

#include "../../../agent/data/data.hh"
#include "../../../agent/data/template/tdata.hh"
#include "../../../agent/data/primitive/primitiveData.hh"

#include "../../../projects/chemotaxis_population/cellparameterinitializer.hh"

#include "../../simulator.hh"
#include "../../../util/util.hh"

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
		~ChemotaxisMovementSimulatorInWorld();

		void setWorldAgent(Agent *ag);

		void step(double t); // t is the timestep that we need to
		void prepare();
		void synchroniseWithDatabase();

	protected:
		void initialise();

	private:
		// not quite sure what these do, i assume
		void changeDirRandom(int i);
		void changeDirDistribution(int i);
		void changeDirRotDiffusion(int i);
		void swimToNewPosition(int i); // what are the parameters ?!
		void moveToNewPosition(int i);

		double eqTime;

		double internalTime;

		// here we declare all the pointers to the data-structures of the world that are needed for the simulation
		IntegerData *numberofcells;
		DoubleData  *rotdiffconst;
		DoubleData  *movement_dt;

		TVectorData<double> *cellspeeds;
		TVectorData<int>    *last_swimming_states;
		TVectorData<int>    *swimming_states;

		TVectorData<TVectorData<double>* > *cell_positions;
		TVectorData<TVectorData<double>* > *cell_dir_vecs;
		TVectorData<TVectorData<double>* > *cell_up_vecs;

		double rotMat[3][3];
		Util::GammaSampler *gs;
};



}



#endif /* CHEMOTAXISMOVEMENTINWORLD_HH_ */
