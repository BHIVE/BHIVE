/**
 * @file SC_composers
 *
 *this file contains the interface to the composers of the simpleCell simulations.
 *
 *@date Jan 29, 2010
 *@author chenry,rcolasanti
 */

#include "../../../composer/composer.hh"
#include "../../../propagator_class.hh"
#include "../../../communicator/communicator.hh"

#include <iostream>
#include <fstream>

#ifndef _SC_PARALLELCOMPOSERS_H_
#define _SC_PARALLELCOMPOSERS_H_


using namespace std;
using namespace Hive;


namespace SimpleCell {
	class SimpleCellDefaultParallelComposer : public Composer {//scott,fangfang
	public:
		SimpleCellDefaultParallelComposer(string inputFileName, string outputFileName, double eqTime);
		~SimpleCellDefaultParallelComposer();
		void setupSimulation();
		void addSerialCommunicator();
		void outputNow(double currentTime);
		void updateAllChemicalConc(double c);
	protected:
		//prepare agent factories for agent setup
		void initializeAgentFactories();
		//establishes child parent relationships among agents
		void setupAgentHierarchy();
		//creates an agent, @param af agentfactory that does the actual creation of agents., @return pointer to created agent
		Agent* createAgent(AgentFactory& af);
		void parseInputFile(string inputFileName);
        void parseInputFileBuffer(vector <string> &buffer);

		// vectors to store cell data for each cell
        string inputFileName;
        int total_number_of_cells;
        
		vector <double> meanCheYp;
		vector <double> timescale;
		vector <double> noise;
		vector <int> numberOfMotors;
		vector <int> numberOfCells;
		vector <int> isMotorCorrelated;
		int environmentType;
		double envParam1;
		double envParam2;
		ofstream outputFile;
		double eqTime;
	};
}

#endif /* _SC_PARALLELCOMPOSERS_H_ */

