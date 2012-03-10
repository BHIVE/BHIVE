/**
 * @file SC_composers
 *
 *this file contains the interface to the composers of the simpleCell simulations.
 *
 *@date Jan 29, 2010
 *@author michael sneddon
 */



#include "../../../composer/composer.hh"
#include <iostream>
#include <fstream>

#ifndef SC_COMPOSERS_HH_
#define SC_COMPOSERS_HH_


using namespace std;
using namespace Hive;


namespace SimpleCell {


	class SimpleCellDefaultComposer : public Composer {
	public:
		SimpleCellDefaultComposer(string inputFileName, string outputFileName, double eqTime);
		~SimpleCellDefaultComposer();
		virtual void setupSimulation();
		void addSerialCommunicator();


		void outputNow(double currentTime);

		void updateAllChemicalConc(double c);


	protected:

		/**
		 *	prepare agent factories for agent setup
		 */
		virtual void initializeAgentFactories();

		/**
		 * establishes child parent relationships among agents
		 */
		virtual void setupAgentHierarchy();



		/**
		 * creates an agent
		 * @param af agentfactory that does the actual creation of agents.
		 * @return pointer to created agent
		 */
		virtual Agent* createAgent(AgentFactory& af);


		void parseInputFile(string inputFileName);


		// vectors to store cell data for each cell
		vector <double> meanCheYp;
		vector <double> timescale;
		vector <double> noise;
		vector <int> numberOfMotors;
		vector <int> numberOfCells;
		vector <int> isMotorCorrelated;
		int environmentType;
		double envParam1;
		double envParam2;


		Hive::Communicator *communicator;

		ofstream outputFile;

		double eqTime;



	};



}

#endif /* SC_COMPOSER_HH_ */
