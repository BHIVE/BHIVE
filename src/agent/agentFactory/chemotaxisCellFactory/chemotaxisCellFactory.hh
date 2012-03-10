/**
 * @file chemotaxisCellFactory.hh
 *
 * This class defines the ChemotaxisCellFactory that creates chemotaxis
 * cell agents with a movement simulator and a basic flux balance simulator
 *
 *
 * @date Oct 21, 2009  Last edited: Oct 23, 2009 Michael, Garrit, Chris
 * @author Michael Sneddon
 */

#ifndef CHEMOTAXISCELLFACTORY_HH_
#define CHEMOTAXISCELLFACTORY_HH_


#include "../agent_factory.hh"
#include "../../agent.hh"
#include "../../action/arnie.hh"


namespace Chemotaxis
{

	/**
	 * @brief creates motile, chemical consuming and secreting, bacteria agents
	 *  @author Michael Sneddon
	 *  @date Oct 21, 2009
	 */
	class ChemotaxisCellFactory : public Hive::AgentFactory
	{
		public:

			/// default constructor
			ChemotaxisCellFactory();
			ChemotaxisCellFactory(double speed, double rotDiffusionConst);

			/// destructor
			virtual ~ChemotaxisCellFactory();

			Hive::Agent *createAgent();

			Hive::Agent *createAgent(Hive::Action ** as);

			Hive::Agent *duplicateAgent(Hive::Agent *ag);

			void finalise();


		protected:
			/**
			 * adds an action to the agents action set
			 * @param *ag pointer to agent
			 */
			void addActionsToAgentsActionSet(Hive::Agent *ag);

			/**
			 * adds a simulator to the agents simulator set
			 * @param *s pointer to simulator
			 */
			void addSimulatorToAgent(Hive::Agent *ag);

		private:
			double speed;
			double rotDiffConst;
			double startX;
			double startY;
			double startZ;
			double movementDt;

	};







}


#endif /* CHEMOTAXISCELLFACTORY_HH_ */
