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
	 *        note that this simulator also takes care of the dissipation of heat in the environment.
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
		int	number_of_elements;
		vector<double> coeffs;
		vector<double> diss;
		vector< TVectorData<TVectorData<DoubleData>* > > elements;
		TVectorData<TVectorData<DoubleData>* > *temperature_grid;
		TVectorData<TVectorData<double>* > *tmp_grid;
		TVectorData<StringData> *element_names;
};


}

#endif /* DIFFUSION_SIMULATOR_HH_ */
