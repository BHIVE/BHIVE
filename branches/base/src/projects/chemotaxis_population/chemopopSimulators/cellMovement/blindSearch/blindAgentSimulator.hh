/*
 * blindAgentSimulator.hh
 *
 *  Created on: Jun 16, 2010
 *      Author: jentsch, colasanti
 */

#ifndef BLINDAGENTSIMULATOR_HH_
#define BLINDAGENTSIMULATOR_HH_

#include <cmath>

#include "../../../../../core/agent/data/data.hh"
#include "../../../../../core/agent/data/template/tdata.hh"
#include "../../../../../core/agent/data/primitive/primitiveData.hh"

#include "../../../chemopopMessageGenerator/chemopopMessageGenerator.hh"
#include "../../../chemopopInitialiser/cellparameterinitializer.hh"

#include "../../../../../core/simulators/simulator.hh"
#include "../../../../../core/util/util.hh"

using namespace std;
using namespace Hive;

namespace ChemoPop {


/**  Blind mono modal search for moving bug in the abscence of chemical field
 *
 * @author ric
 */
class BlindMonoModalAgentSimulator : public Simulator {
public:
	BlindMonoModalAgentSimulator();
	~BlindMonoModalAgentSimulator() {};

	void setAgent(Agent* ag);
	void step(double t);
	void prepare() {};
	void synchroniseWithDatabase() {};

protected:
	void initialise() {};

private:
	/**  Blind mono modal move
	 *  the length of run is determined by a log lin distribution
	 * @param name description
	 * @return description
	 */
	void move();
};

/**  Random walk search (move set distance then change direction)
 *
 * @author ric
 */
class BlindRandomWalkerAgentSimulator : public Simulator {
public:
	BlindRandomWalkerAgentSimulator();
	~BlindRandomWalkerAgentSimulator() {};

	void setAgent(Agent* ag);
	void step(double t);
	void prepare() {};
	void synchroniseWithDatabase() {};

protected:
	void initialise() {};

private:
	void move();
};




}

#endif /* BLINDAGENTSIMULATOR_HH_ */
