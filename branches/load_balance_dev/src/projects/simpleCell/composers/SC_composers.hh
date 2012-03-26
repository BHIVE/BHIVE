/**
 * @file SC_composers
 *
 *this file contains the interface to the composers of the simpleCell simulations.
 *
 *@date Jan 29, 2010
 *@author chenry,rcolasanti
 */

#ifndef SC_COMPOSERS_HH_
#define SC_COMPOSERS_HH_

#include "../universe/universe.hh"
#include "../cartesianGrid/cartesianGrid.hh"
#include "../bactCell/bactCell.hh"
#include "../../../composer/composer.hh"
#include "../../../propagator_class.hh"
#include "../../../communicator/communicator.hh"

#include <iostream>
#include <fstream>


using namespace std;
using namespace Hive;

namespace SimpleCell {

class UniverseDatabase;

class SimpleCellDefaultComposer : public Composer {//scott,fangfang
  public:
    SimpleCellDefaultComposer();
    ~SimpleCellDefaultComposer();
    void compose(string inputFileName, string outputFileName);
    void parallelCompose(string inputFileName, string outputFileName);
    void configureMyWorld(int rank, int size, int xLen, int yLen, int zLen, double blockSize,
                          vector<int>& myLens, vector<double>& startingPosition, UniverseDatabase* univDB);
    void setupSimulation();
    void addCommunicator();
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
    ofstream outputFile;
    double eqTime;
};

}

#endif /* SC_COMPOSER_HH_ */
