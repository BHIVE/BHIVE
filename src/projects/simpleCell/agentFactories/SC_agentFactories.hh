/**
 * SC_agentFactories.hh
 *
 *  Created on: Jan 29, 2010
 *      Author: msneddon
 */

#ifndef SC_AGENTFACTORIES_HH_
#define SC_AGENTFACTORIES_HH_



//
//#include "../../agent/agentFactory/agent_factory.hh"
//#include "diffusion_data.hh"
//#include "../../input/meshinputreader/netgenmeshinputreader.hh"
//#include "../../simulators/pde/PDESimulator.hh"
//#include "../../agent/messageGenerator/chemotaxisMessageGenerator/chemotaxisMessageGenerator.hh"
//
//#include "../../agent/data/grid/nvoxel.hh"
//#include "../../agent/data/grid/grid.hh"

#include "../../../agent/agentFactory/agent_factory.hh"
#include <stdlib.h>


using namespace std;
using namespace Hive;

namespace SimpleCell {


	class SimpleCellFactory : public AgentFactory {
		public:
			SimpleCellFactory(const int signalType, const int motorType, const int motionType, double eqTime);
			SimpleCellFactory(InputSystemReader *isr);
			~SimpleCellFactory();

			void finalise();

			Agent *createAgent() { cout<<"cannot call SimpleCell::createAgent()"<<endl; exit(1); };
			Agent *createAgent(double meanCheYp, double tau, double noise, int nMotors, bool areMotorsCorrelated);
			Agent *createAgent(Action **ac);
			Agent *duplicateAgent(Agent *ag);

		protected:

			void addActionsToAgentsActionSet(Agent *ag) {};
			void addSimulatorToAgent(Agent *ag) {cout<<"cannot call SimpleCell::addSimulatorToAgent(Agent *ag)"<<endl; exit(1); };

			void addSimulatorToAgent(Agent *ag,	double meanCheYp, double tau, double noise, int nMotors, bool areMotorsCorrelated,double eqTime);

			void addMessageGenerators(Agent *ag) {};

		private:
			int signalingSimulatorType;
			int motorSimulatorType;
			int motionSimulatorType;

			double eqTime;
	};



	class EnvironmentAgentFactory : public AgentFactory {
		public:
			EnvironmentAgentFactory(const int type);
			EnvironmentAgentFactory(InputSystemReader *isr);
			~EnvironmentAgentFactory();

			void finalise();

			Agent *createAgent();
			Agent *createAgent(Action **ac);
			Agent *duplicateAgent(Agent *ag);

		protected:

			void addActionsToAgentsActionSet(Agent *ag) {};
			void addSimulatorToAgent(Agent *ag) {};

			void addMessageGenerators(Agent *ag) {};

		private:
			int simulatorType;
	};



}

#endif /* SC_AGENTFACTORIES_HH_ */
