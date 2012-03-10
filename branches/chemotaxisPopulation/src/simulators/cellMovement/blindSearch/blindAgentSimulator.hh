/*
 * blindAgentSimulator.hh
 *
 *  Created on: Jun 16, 2010
 *      Author: jentsch, colasanti
 */

#ifndef BLINDAGENTSIMULATOR_HH_
#define BLINDAGENTSIMULATOR_HH_

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
