/*
 * transwellcomposer.hh
 *
 *  Created on: Dec 10, 2009
 *      Author: jentsch
 */

#ifndef TRANSWELLCOMPOSER_HH_
#define TRANSWELLCOMPOSER_HH_

#include "../../composer/composer.hh"
#include "worldagentfactory.hh"
#include "tcellfactory.hh"
#include "nucleusagentfactory.hh"
#include "outputagentfactory.hh"
#include "../../communicator/serial/serialcommunicator.hh"

#include <iostream>

using namespace std;
using namespace Hive;

namespace Transwell {
/**
 * @brief composer for setting up the transwell simulation
 *
 * @author Garrit Jentsch
 *
 * @date Dec 10th, 2009  last edited: 1-12-2010 by garrit
 */
	class TranswellComposer : public Composer {
	public:
		TranswellComposer();
		~TranswellComposer();

		void setupNumberOfCells(int);

		void setupSimulation();

	protected:
		void initializeAgentFactories();

		void setupAgentHierarchy();

		void addSerialCommunicator();

		Agent* createAgent(AgentFactory& af);

		int number_of_cells;

		SerialCommunicator *post;
	};


}
#endif /* TRANSWELLCOMPOSER_HH_ */
