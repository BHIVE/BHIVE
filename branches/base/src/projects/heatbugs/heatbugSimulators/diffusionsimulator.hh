/*
 *  diffusion_simulator.hh
 *
 *  Created on: Jan 5, 2011
 *      Author: jentsch
 */

#ifndef DIFFUSION_SIMULATOR_HH_
#define DIFFUSION_SIMULATOR_HH_

#include "../../../core/simulators/simulator.hh"
#include "../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../core/agent/data/template/tdata.hh"

#include <cmath>
#include <iostream>
#include <vector>

using namespace std;
using namespace Hive;

namespace Heatbug {

	/*
	 * @brief simple simulator for simulating diffusion on a 2D grid for the heatbug application.
	 *        NOTE THAT THIS SIMULATOR ALSO TAKES CARE OF THE DISSIPATION OF HEAT IN THE ENVIRONMENT
	 *        IT IS THEREFORE NOT A PURE DIFFUSION SIMULATOR!
	 *
	 * @author Garrit Jentsch
	 *
	 * @date Jan 5th, 2011
	 */
	class DiffusionSimulator : public Simulator {
	public:
		DiffusionSimulator();

		~DiffusionSimulator();

		void step(double t);

		void prepare();

		void setAgent(Agent *ag);

		void setTimeStep(double);

		void synchroniseWithDatabase();

	protected:
		void initialise();

	private:
		// timestep
		double dt;

		double diffCoeff;
		double dissCoeff;
		double dx;
		double numCoeff;
		int    gridsize;
		TVectorData<TVectorData<DoubleData>* > *temperature_grid;
		TVectorData<TVectorData<double>* > *tmp_grid;

};


}

#endif /* DIFFUSION_SIMULATOR_HH_ */
