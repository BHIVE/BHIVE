/*
 * blindagentmovementsimulator.hh
 *
 *  Created on: Jul 7, 2010
 *      Author: jentsch
 */

#ifndef BLINDAGENTMOVEMENTSIMULATOR_HH_
#define BLINDAGENTMOVEMENTSIMULATOR_HH_

#include <cmath>

#include "../../../agent/data/data.hh"
#include "../../../agent/data/template/tdata.hh"
#include "../../../agent/data/primitive/primitiveData.hh"

#include "../../../agent/messageGenerator/chemopopMessageGenerator/chemopopMessageGenerator.hh"
#include "../../../projects/chemotaxis_population/cellparameterinitializer.hh"

#include "../../simulator.hh"
#include "../../../util/util.hh"

using namespace std;
using namespace Hive;

namespace ChemoPop {
	// abstract base class from which we are going to derive
	// the movement simulators
	class BlindAgentMovementSimulator : public Simulator {
	public:
		/// constructor
		BlindAgentMovementSimulator();
		/// virtual destructor
		virtual ~BlindAgentMovementSimulator() {};
		/// setting the agent
		void setAgent(Agent* ag);
		/// virtual step method
		virtual void step(double t);
		/// virtual step method
		virtual void step(Agent *ag, double t);
		/// preparation method
		void prepare() {};
		/// synchronisation with database
		void synchroniseWithDatabase() {};

	protected:
		/// initialisation method
		virtual void initialise();
		/// draws a new angle from wrapped
		virtual void drawNewAngle();
		/// draw new distance to travel.
		virtual void drawNewDistance() = 0;
		/// ptr to the relevant items of the agent's database
		DoubleData *rho;
		DoubleData *traveled_distance;
		DoubleData *desired_distance_to_travel;
		DoubleData *velocity;
		DoubleData *current_angle;
		DoubleData *parameter_for_steplength_dist;
	};
};

#endif /* BLINDAGENTMOVEMENTSIMULATOR_HH_ */
